#pragma once

#include <ncursesw/ncurses.h>
#include <ncursesw/menu.h>

#include "IRunnable.hpp"
#include "MenuSubwindow.hpp"
#include "WindowView.hpp"

class WindowView;
class MenuSubwindow;

class MenuView : public WindowView, public IRunnable {
public:
    using ptr = std::shared_ptr<MenuView>;

    static MenuView::ptr create(Size size, Position position);
    static MenuView::ptr create(unsigned height, unsigned width, unsigned start_y, unsigned start_x);
    static MenuView::ptr create(Size size, Position position, 
                                TextW title, unsigned title_height);
    static MenuView::ptr create(unsigned height, unsigned width, unsigned start_y, unsigned start_x, 
                                TextW title, unsigned title_height);
    static MenuView::ptr create(BaseWindow::ptr parent, Size size, Position local_position = Position(0, 0));
    static MenuView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y = 0, unsigned start_x = 0);
    static MenuView::ptr create(BaseWindow::ptr parent, Size size, Margins margins = Margins(0, 0, 0, 0));
    static MenuView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top = 0, unsigned margin_bottom = 0, unsigned margin_left = 0, unsigned margin_right = 0);
    static MenuView::ptr create(BaseWindow::ptr parent, Size size, Position local_position, 
                                TextW title, unsigned title_height);
    static MenuView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x, 
                                TextW title, unsigned title_height);
    static MenuView::ptr create(BaseWindow::ptr parent, Size size, Margins margins, 
                                TextW title, unsigned title_height);
    static MenuView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, 
                                unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right, 
                                TextW title, unsigned title_height);

    MenuView(const MenuView& other) = delete;
    MenuView& operator=(const MenuView& other) = delete;
    MenuView(MenuView&& other) = delete;
    MenuView& operator=(MenuView&& other) = delete;
    ~MenuView() override;

    void set_menu_columns(int columns);
    void set_menu_margins(unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right);

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
    void add_menu_item(const std::wstring& item, const std::wstring& desc = L"", std::function<void()> callback = nullptr, std::vector<wint_t> keys = {});
    void add_exit_callback(std::function<void()> callback = nullptr, std::vector<wint_t> keys = {});

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
    MenuSubwindow::ptr menu_window;

    MenuView(Size size, Position position);
    MenuView(Size size, Position position, TextW title, unsigned title_height);
    MenuView(BaseWindow::ptr parent, Size size, Position local_position);
    MenuView(BaseWindow::ptr parent, Size size, Margins margins);
    MenuView(BaseWindow::ptr parent, Size size, Position local_position, TextW title, unsigned title_height);
    MenuView(BaseWindow::ptr parent, Size size, Margins margins, TextW title, unsigned title_height);

    void set_listeners_for_inner_window_events();
};