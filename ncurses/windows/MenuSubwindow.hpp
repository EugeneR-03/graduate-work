#pragma once

#include <ncursesw/ncurses.h>
#include <ncursesw/menu.h>

#include "IRunnable.hpp"
#include "BaseWindow.hpp"

class MenuSubwindow : public BaseWindow, public IRunnable {
public:
    using ptr = std::shared_ptr<MenuSubwindow>;

    static MenuSubwindow::ptr create(BaseWindow::ptr parent, Size size, Position local_position);
    static MenuSubwindow::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x);
    static MenuSubwindow::ptr create(BaseWindow::ptr parent, Size size, Margins margins);
    static MenuSubwindow::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right);
    
    MenuSubwindow(const MenuSubwindow& other) = delete;
    MenuSubwindow& operator=(const MenuSubwindow& other) = delete;
    MenuSubwindow(MenuSubwindow&& other) = delete;
    MenuSubwindow& operator=(MenuSubwindow&& other) = delete;
    ~MenuSubwindow() override;

    void set_unfocused_border(std::shared_ptr<IBorder> unfocused_border) = delete;
    void set_focused_border(std::shared_ptr<IBorder> inactive_border) = delete;
    void set_active_border(std::shared_ptr<IBorder> active_border) = delete;
    
    void set_menu_columns(int columns);

    void set_custom_menu_mark(const std::wstring& mark);
    void set_active_items_attrs(chtype attrs);
    void set_inactive_items_attrs(chtype attrs);
    void set_unfocused_items_attrs(chtype attrs);
    void set_defined_item_attrs(chtype attrs);

    int get_defined_item_index();
    void set_defined_item(int index);
    void reset_defined_item();

    int get_current_item_index();

    void add_menu_item(const std::string& item, const std::string& desc = "", Callback callback = nullptr, std::vector<wint_t> keys = {});
    void add_menu_item(const std::wstring& item, const std::wstring& desc = L"", Callback callback = nullptr, std::vector<wint_t> keys = {});
    void add_exit_callback(Callback callback = nullptr, std::vector<wint_t> keys = {});

    void move_to(unsigned new_y, unsigned new_x) override;
    void resize(unsigned height, unsigned width) override;
    void update(unsigned height, unsigned width, unsigned start_y, unsigned start_x) override;
    void show() override;
    void hide() override;
    void set_focus(bool on) override;

    void run() override;
    void stop() override;
    bool is_running() override;

protected:
    MENU_ptr menu;
    std::wstring menu_mark;
    char* menu_mark_char;

    std::vector<char*> menu_items_names;
    std::vector<char*> menu_items_descs;
    std::vector<ITEM*> menu_items;
    std::vector<KeysCallback> menu_callbacks;
    std::vector<KeysCallback> exit_callbacks;

    MenuSubwindow(BaseWindow::ptr parent, Size size, Position local_position);
    MenuSubwindow(BaseWindow::ptr parent, Size size, Margins margins);

    int get_max_item_length();
    void set_item_style(int index, chtype attrs);
    void set_styles();
    void draw_mark(int row, bool on);
    void draw_scrollbar();
    bool execute_exit_callback(wint_t ch);
    bool execute_menu_callback(int index, wint_t ch);

    bool custom_menu_driver(int ch);
    void draw_menu(bool is_focused);
    void update_menu();

private:
    int menu_columns = 1;
    int current_item_index = 0;
    int defined_item_index = 0;
    int max_item_length = 0;
    int menu_mark_length = 0;
    chtype active_item_attrs = A_REVERSE;
    chtype inactive_item_attrs = A_NORMAL;
    chtype unfocused_item_attrs = A_NORMAL;
    chtype defined_item_attrs = A_NORMAL;
    bool is_running_ = false;
};