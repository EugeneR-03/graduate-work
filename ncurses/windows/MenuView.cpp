#include "MenuView.hpp"

MenuView::MenuView(Size size, Position position)
    : WindowView(size, position) {}

MenuView::MenuView(Size size, Position position, TextW title, unsigned title_height)
    : WindowView(size, position) {}

MenuView::MenuView(BaseWindow::ptr parent, Size size, Position local_position)
    : WindowView(parent, size, local_position) {}

MenuView::MenuView(BaseWindow::ptr parent, Size size, Margins margins)
    : WindowView(parent, size, margins) {}

MenuView::MenuView(BaseWindow::ptr parent, Size size, Position local_position, TextW title, unsigned title_height)
    : WindowView(parent, size, local_position) {}

MenuView::MenuView(BaseWindow::ptr parent, Size size, Margins margins, TextW title, unsigned title_height)
    : WindowView(parent, size, margins) {}

MenuView::~MenuView() {}

MenuView::ptr MenuView::create(Size size, Position position)
{
    auto window = MenuView::ptr(new MenuView(size, position));
    window->menu_window = MenuSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    return window;
}

MenuView::ptr MenuView::create(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    auto window = MenuView::ptr(new MenuView(Size(height, width), Position(start_y, start_x)));
    window->menu_window = MenuSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    return window;
}

MenuView::ptr MenuView::create(Size size, Position position, 
                               TextW title, unsigned title_height)
{
    auto window = MenuView::ptr(new MenuView(size, position, title, title_height));
    window->init_title_window(title, title_height);
    window->menu_window = MenuSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    return window;
}

MenuView::ptr MenuView::create(unsigned height, unsigned width, unsigned start_y, unsigned start_x, 
                               TextW title, unsigned title_height)
{
    auto window = MenuView::ptr(new MenuView(Size(height, width), Position(start_y, start_x), title, title_height));
    window->init_title_window(title, title_height);
    window->menu_window = MenuSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    return window;
}

MenuView::ptr MenuView::create(BaseWindow::ptr parent, Size size, Position local_position)
{
    if (!parent) {
        throw std::runtime_error("MenuView::create(): Parent window does not exist");
    }

    auto window = MenuView::ptr(new MenuView(parent, size, local_position));
    window->menu_window = MenuSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    parent->add_child(window);
    return window;
}

MenuView::ptr MenuView::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    if (!parent) {
        throw std::runtime_error("MenuView::create(): Parent window does not exist");
    }

    auto window = MenuView::ptr(new MenuView(parent, Size(height, width), Position(start_y, start_x)));
    window->menu_window = MenuSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    parent->add_child(window);
    return window;
}

MenuView::ptr MenuView::create(BaseWindow::ptr parent, Size size, Position local_position, 
                               TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("MenuView::create(): Parent window does not exist");
    }

    auto window = MenuView::ptr(new MenuView(parent, size, local_position, title, title_height));
    window->init_title_window(title, title_height);
    window->menu_window = MenuSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    parent->add_child(window);
    return window;
}

MenuView::ptr MenuView::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x, 
                               TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("MenuView::create(): Parent window does not exist");
    }

    auto window = MenuView::ptr(new MenuView(parent, Size(height, width), Position(start_y, start_x), title, title_height));
    window->init_title_window(title, title_height);
    window->menu_window = MenuSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    parent->add_child(window);
    return window;
}

MenuView::ptr MenuView::create(BaseWindow::ptr parent, Size size, Margins margins, 
                               TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("MenuView::create(): Parent window does not exist");
    }

    auto window = MenuView::ptr(new MenuView(parent, size, margins, title, title_height));
    window->init_title_window(title, title_height);
    window->menu_window = MenuSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    parent->add_child(window);
    return window;
}

MenuView::ptr MenuView::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right,
                               TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("MenuView::create(): Parent window does not exist");
    }

    auto window = MenuView::ptr(new MenuView(parent, Size(height, width), Margins(margin_top, margin_bottom, margin_left, margin_right), title, title_height));
    window->init_title_window(title, title_height);
    window->menu_window = MenuSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    parent->add_child(window);
    return window;
}

void MenuView::set_menu_columns(int columns)
{
    menu_window->set_menu_columns(columns);
}

void MenuView::set_menu_margins(unsigned top, unsigned bottom, unsigned left, unsigned right)
{
    menu_window->set_margins(top, bottom, left, right);
}

void MenuView::set_custom_menu_mark(const std::wstring& mark)
{
    menu_window->set_custom_menu_mark(mark);
}

void MenuView::set_active_items_attrs(chtype attrs)
{
    menu_window->set_active_items_attrs(attrs);
}

void MenuView::set_inactive_items_attrs(chtype attrs)
{
    menu_window->set_inactive_items_attrs(attrs);
}

void MenuView::set_unfocused_items_attrs(chtype attrs)
{
    menu_window->set_unfocused_items_attrs(attrs);
}

void MenuView::set_defined_item_attrs(chtype attrs)
{
    menu_window->set_defined_item_attrs(attrs);
}

int MenuView::get_defined_item_index()
{
    return menu_window->get_defined_item_index();
}

void MenuView::set_defined_item(int index)
{
    menu_window->set_defined_item(index);
}

void MenuView::reset_defined_item()
{
    menu_window->reset_defined_item();
}

int MenuView::get_current_item_index()
{
    return menu_window->get_current_item_index();
}

void MenuView::add_menu_item(const std::string& item, const std::string& desc, Callback callback, std::vector<wint_t> keys)
{
    menu_window->add_menu_item(item, desc, callback, keys);
}

void MenuView::add_menu_item(const std::wstring& item, const std::wstring& desc, Callback callback, std::vector<wint_t> keys)
{
    menu_window->add_menu_item(item, desc, callback, keys);
}

void MenuView::add_exit_callback(Callback callback, std::vector<wint_t> keys)
{
    menu_window->add_exit_callback(callback, keys);
}

void MenuView::move_to(unsigned new_y, unsigned new_x)
{
    WindowView::move_to(new_y, new_x);
}

void MenuView::resize(unsigned height, unsigned width)
{
    WindowView::resize(height, width);
}

void MenuView::update(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    WindowView::update(height, width, start_y, start_x);
}

void MenuView::show()
{
    WindowView::show();
    menu_window->show();
}

void MenuView::hide()
{
    WindowView::hide();
    menu_window->stop();
    menu_window->hide();
}

void MenuView::set_focus(bool on)
{
    WindowView::set_focus(on);
    menu_window->set_focus(on);
}

void MenuView::run()
{
    menu_window->run();
}

void MenuView::stop()
{
    menu_window->stop();
}

bool MenuView::is_running()
{
    return menu_window->is_running();
}

void MenuView::set_listeners_for_inner_window_events()
{
    menu_window->on_resize += [this]() {
        on_resize();
    };
    menu_window->on_move += [this]() {
        on_move();
    };
    menu_window->on_show += [this]() {
        on_show();
    };
    menu_window->on_hide += [this]() {
        on_hide();
    };
    menu_window->on_idle += [this]() {
        on_idle();
    };
    menu_window->on_focus += [this]() {
        on_focus();
    };
    menu_window->on_unfocus += [this]() {
        on_unfocus();
    };
}
