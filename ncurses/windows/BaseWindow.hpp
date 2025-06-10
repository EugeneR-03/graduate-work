#pragma once

#include <vector>
#include <ncursesw/ncurses.h>
#include <ncursesw/panel.h>
#include <ncursesw/menu.h>
#include <ncursesw/form.h>

#include "./../base/BaseStructures.hpp"
#include "./../base/Borders.hpp"
#include "./../../base/Flags.hpp"
#include "./../../base/Event.hpp"

using Callback = std::function<void()>;
using KeyCallback = std::pair<wint_t, Callback>;
using KeysCallback = std::pair<std::vector<wint_t>, Callback>;

#include "./../NcursesUtils.hpp"

using namespace Flags::EnumFlags;

struct NcursesWindowDeleter {
    void operator()(WINDOW* win) const {
        if (win) {
            delwin(win);
        }
    }
};

struct NcursesPanelDeleter {
    void operator()(PANEL* panel) const {
        if (panel) {
            del_panel(panel);
        }
    }
};

struct NcursesFormDeleter {
    void operator()(FORM* form) const {
        if (form) {
            unpost_form(form);
            free_form(form);
        }
    }
};

struct NcursesMenuDeleter {
    void operator()(MENU* menu) const {
        if (menu) {
            unpost_menu(menu);
            free_menu(menu);
        }
    }
};

using WINDOW_ptr = std::unique_ptr<WINDOW, NcursesWindowDeleter>;
using PANEL_ptr = std::unique_ptr<PANEL, NcursesPanelDeleter>;
using FORM_ptr = std::unique_ptr<FORM, NcursesFormDeleter>;
using MENU_ptr = std::unique_ptr<MENU, NcursesMenuDeleter>;

class BaseWindow : public std::enable_shared_from_this<BaseWindow> {
public:
    using ptr = std::shared_ptr<BaseWindow>;

    enum State : unsigned short {
        Hidden = 0,
        Showing = 1 << 0,
        Focused = 1 << 1
    };

    static BaseWindow::ptr create(Size size, Position position);
    static BaseWindow::ptr create(unsigned height, unsigned width, unsigned start_y, unsigned start_x);
    static BaseWindow::ptr create(BaseWindow::ptr parent, Size size, Position local_position = Position(0, 0));
    static BaseWindow::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y = 0, unsigned start_x = 0);
    static BaseWindow::ptr create(BaseWindow::ptr parent, Size size, Margins margins = Margins(0, 0, 0, 0));
    static BaseWindow::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top = 0, unsigned margin_bottom = 0, unsigned margin_left = 0, unsigned margin_right = 0);
    
    BaseWindow(const BaseWindow& other) = delete;
    BaseWindow& operator=(const BaseWindow& other) = delete;
    BaseWindow(BaseWindow&& other) = delete;
    BaseWindow& operator=(BaseWindow&& other) = delete;
    virtual ~BaseWindow();

    std::vector<BaseWindow::ptr> get_children() const;
    void add_child(BaseWindow::ptr child);
    void remove_child(BaseWindow::ptr child);
    void remove_child(BaseWindow* child);
    
    BaseWindow* get_parent() const;
    void set_parent(BaseWindow::ptr new_parent, unsigned local_start_y = 0, unsigned local_start_x = 0);
    
    WINDOW* get_win() const;
    PANEL* get_panel() const;
    State get_state() const;

    Size get_size() const;
    unsigned get_width() const;
    unsigned get_height() const;

    Position get_position() const;
    unsigned get_start_y() const;
    unsigned get_start_x() const;

    Margins get_margins() const;
    unsigned get_margins_top() const;
    unsigned get_margins_bottom() const;
    unsigned get_margins_left() const;
    unsigned get_margins_right() const;
    void set_margins(const Margins& margins);
    void set_margins(unsigned top, unsigned bottom, unsigned left, unsigned right);
    
    std::shared_ptr<IBorder> get_unfocused_border() const;
    void set_unfocused_border(std::shared_ptr<IBorder> border);
    std::shared_ptr<IBorder> get_focused_border() const;
    void set_focused_border(std::shared_ptr<IBorder> border);
    std::shared_ptr<IBorder> get_active_border() const;
    void set_active_border(std::shared_ptr<IBorder> border);
    
    virtual void resize(unsigned height, unsigned width);
    virtual void move_to(unsigned new_y, unsigned new_x);
    virtual void update(unsigned height, unsigned width, unsigned start_y, unsigned start_x);
    virtual void show();
    virtual void hide();
    virtual void set_focus(bool on);

    void move_to_and_show(int new_y, int new_x);
    void resize_and_show(int height, int width);
    void update_and_show(int height, int width, int start_y, int start_x);

    bool is_showing() const;
    bool is_focused() const;

    Event<void()> on_resize;
    Event<void()> on_move;
    Event<void()> on_show;
    Event<void()> on_hide;
    Event<void()> on_idle;
    Event<void()> on_focus;
    Event<void()> on_unfocus;
    Event<void(int y, int x)> on_hover;
    Event<void()> on_unhover;
    Event<void(int y, int x)> on_left_click;
    Event<void(int y, int x)> on_left_press;
    Event<void(int y, int x)> on_left_release;
    Event<void(int y, int x)> on_right_click;

protected:
    std::weak_ptr<BaseWindow> parent;
    std::vector<BaseWindow::ptr> children = {};
    bool parent_borders_considered = false;

    WINDOW_ptr win;
    PANEL_ptr panel;
    Size size = Size(0, 0);
    Size size_with_margins = Size(0, 0);
    Position position = Position(0, 0);
    Margins margins = Margins(0, 0, 0, 0);  // margins for parent
    State state = State::Hidden;

    std::shared_ptr<IBorder> active_border = nullptr;
    std::shared_ptr<IBorder> unfocused_border = nullptr;
    std::shared_ptr<IBorder> focused_border = nullptr;

    BaseWindow(Size size, Position position);
    BaseWindow(BaseWindow::ptr parent, Size size, Position local_position);
    BaseWindow(BaseWindow::ptr parent, Size size, Margins margins);

    void init();
    void del();
    void update_layout();

    unsigned get_border_length() const;

    static Size calculate_size_with_margins(const Size& size, const Margins& margins);
    static Size calculate_size_with_margins_and_borders(const Size& size, const Margins& margins, Margins border_length = Margins(0, 0, 0, 0));
    static Position calculate_position_with_margins(const Position& position, const Margins& margins);
    static Position calculate_position_with_margins_and_borders(const Position& position, const Margins& margins, Margins border_length = Margins(0, 0, 0, 0));
};