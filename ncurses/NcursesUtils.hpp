#pragma once

#include <ncursesw/ncurses.h>

#include <stdexcept>
#include <string>
#include <sstream>
#include <unordered_map>
#include <deque>
#include <cstring>
#include <codecvt>
#include <typeinfo>
#include <iostream>
#include <boost/nowide/convert.hpp>
#include <tsl/hopscotch_map.h>

#include "./base/BaseStructures.hpp"
#include "./../base/Flags.hpp"
#include "./windows/IRunnable.hpp"
#include "./windows/BaseWindow.hpp"

#ifdef _WIN32
    #include <windows.h>
#endif


#define K_C_ENTER       10
#define K_C_BACKSPACE   8
#define K_C_ESCAPE      27

#define K_UTF8_ENTER    0x0A
#define K_UTF8_BACKSPACE 0x8
#define K_UTF8_ESCAPE   0x1B
#define K_UTF8_UP       0x41
#define K_UTF8_DOWN     0x42
#define K_UTF8_LEFT     0x44
#define K_UTF8_RIGHT    0x43
#define K_UTF8_SPACE    0x20
#define K_UTF8_TAB      0x09
#define K_UTF8_DELETE   0x7F
#define K_UTF8_DC       0x7F
#define K_UTF8_HOME     0x48
#define K_UTF8_END      0x46


#ifdef _WIN32
    #define K_ENTER     VK_RETURN
    #define K_ESCAPE    VK_ESCAPE
    #define K_BACKSPACE VK_BACK
    #define K_HOME      VK_HOME
    #define K_END       VK_END
    #define K_DC        VK_DELETE
    #define K_UP        VK_UP
    #define K_DOWN      VK_DOWN
    #define K_LEFT      VK_LEFT
    #define K_RIGHT     VK_RIGHT
#else
    #define K_ENTER     KEY_ENTER
    #define K_ESCAPE    27
    #define K_BACKSPACE KEY_BACKSPACE
    #define K_HOME      KEY_HOME
    #define K_END       KEY_END
    #define K_DC        KEY_DC
    #define K_UP        KEY_UP
    #define K_DOWN      KEY_DOWN
    #define K_LEFT      KEY_LEFT
    #define K_RIGHT     KEY_RIGHT
#endif


/* field options */
// #define O_VISIBLE		(0x0001U)
// #define O_ACTIVE		(0x0002U)
// #define O_PUBLIC		(0x0004U)
// #define O_EDIT			(0x0008U)
// #define O_WRAP			(0x0010U)
// #define O_BLANK			(0x0020U)
// #define O_AUTOSKIP		(0x0040U)
// #define O_NULLOK		(0x0080U)
// #define O_PASSOK		(0x0100U)
// #define O_STATIC		(0x0200U)
// #define O_DYNAMIC_JUSTIFY	(0x0400U)	/* ncurses extension	*/
// #define O_NO_LEFT_STRIP		(0x0800U)	/* ncurses extension	*/
// #define O_EDGE_INSERT_STAY      (0x1000U)	/* ncurses extension	*/
// #define O_INPUT_LIMIT           (0x2000U)	/* ncurses extension	*/
#define O_PASSWORD      (0x4000U)


/* TODO:
    - добавить обработку кликов мышки
    - сейчас обработка кликов для запускаемых окон может создавать много вложенных
      while циклов; они разрываются через windows_manager, но все же
*/


class BaseWindow;

namespace nu {
    using namespace Flags::EnumFlags;
    using WindowsMap = tsl::hopscotch_map<unsigned, tsl::hopscotch_map<unsigned, BaseWindow*>>;

    enum class Mark : unsigned short {
        None    = 0,
        Update  = 1 << 0,
        Redraw  = 1 << 1,
        Run     = 1 << 2
    };

    class WindowsManager {
    public:
        struct WindowPointer {
            BaseWindow* window;
            bool needs_redrawing;

            WindowPointer() : window(nullptr), needs_redrawing(false) {}
            explicit WindowPointer(BaseWindow* window) : window(window), needs_redrawing(false) {}
            explicit WindowPointer(const BaseWindow* window) : window(const_cast<BaseWindow*>(window)), needs_redrawing(false) {}
            explicit WindowPointer(BaseWindow* window, bool needs_redrawing) : window(window), needs_redrawing(needs_redrawing) {}
            explicit WindowPointer(const BaseWindow* window, bool needs_redrawing) : window(const_cast<BaseWindow*>(window)), needs_redrawing(needs_redrawing) {}
        };

        WindowsManager();
        ~WindowsManager();
        void add_window(const BaseWindow* window);
        void remove_window(const BaseWindow* window);
        void remove_window_and_update(const BaseWindow* window);
        void update_window(const BaseWindow* window);
        void top_window(const BaseWindow* window);
        void top_window_and_update(const BaseWindow* window);
        void hide_window(const BaseWindow* window);
        void hide_window_and_update(const BaseWindow* window);
        const BaseWindow* check_presence(unsigned y, unsigned x);
        void mark_window_for_redrawing(const BaseWindow* window, bool needs_redrawing);
        bool needs_redraw(const BaseWindow* window);
        void update();
        void mark_all(Mark mark);
        void show_all();
        void clear();

        IRunnable* get_running_window();
        IRunnable* get_last_running_window();
        int set_running_window(IRunnable* window);
        void reset_running_window();

    private:
        std::deque<WindowPointer> active_windows;     // стек (дек) активных окон
        WindowsMap windows_on_screen;    // аналог разреженной матрицы
        IRunnable* running_window = nullptr;
        IRunnable* last_running_window = nullptr;
    };

    namespace display {
#ifdef _WIN32
        inline HANDLE h_in;
        inline HANDLE h_out;
#endif
        inline Size size(0, 0);
        inline Position center(0, 0);

        inline WindowsManager windows_manager;
        inline Callback redraw_function = nullptr;

        void update_size_and_center();
        void set_redraw_function(Callback callback);
        
        bool needs_redraw(const BaseWindow* window);
        void mark_window(const BaseWindow& window, Mark mark);
        void mark_window(const BaseWindow* window, Mark mark);
        void mark_all_windows(Mark mark);
        void show_all_windows();

        IRunnable* get_running_window();
        IRunnable* get_last_running_window();
        int set_running_window(IRunnable* window);
        void reset_running_window();
    }

    namespace mouse {
        inline Position position(0, 0);
        inline Position previous_position(0, 0);

        void update_mouse_position(unsigned y, unsigned x);
    }

    namespace borders {
        inline std::shared_ptr<BorderW> single_ = std::make_shared<BorderW>(BorderW::Type::Default);
        inline std::shared_ptr<BorderW> thick_ = std::make_shared<BorderW>(BorderW::Type::Thick);
        inline std::shared_ptr<BorderW> double_ = std::make_shared<BorderW>(BorderW::Type::Double);
    }

    void enable_window_events();
    void set_quick_edit_mode(bool on);
    void toggle_quick_edit_mode();
};

namespace nu {
    enum WorkMode : unsigned short {
        None        = 0,
        Default     = 1 << 0,
        WithMouse   = 1 << 1,
    };

    inline WorkMode work_mode = WorkMode::Default;

    void init(WorkMode mode = WorkMode::Default);
    void run();
    void end();

    enum Colors : unsigned short {
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White
    };

    char* wstring_to_char(const std::wstring& string);
    std::wstring char_to_wstring(const char* string);
    std::string wstring_to_string(const std::wstring& string);
    std::wstring string_to_wstring(const std::string& string);
    
    std::string ltrim(const std::string& str);
    std::wstring ltrim(const std::wstring& str);
    std::string rtrim(const std::string& str);
    std::wstring rtrim(const std::wstring& str);

    wchar_t wget_wch(WINDOW* win, WorkMode mode = WorkMode::Default);
    wchar_t get_wch_by_read_console(WorkMode mode = WorkMode::Default);
    void wprint_text(WINDOW* win, TextW title, Size size, Position position);
}
