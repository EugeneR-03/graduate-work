#include "WindowView.hpp"

WindowView::WindowView(Size size, Position position)
    : BaseWindow(size, position) {}

WindowView::WindowView(Size size, Position position, TextW title, unsigned title_height)
    : BaseWindow(size, position) {}

WindowView::WindowView(BaseWindow::ptr parent, Size size, Position local_position)
    : BaseWindow(parent, size, local_position) {}

WindowView::WindowView(BaseWindow::ptr parent, Size size, Margins margins)
    : BaseWindow(parent, size, margins) {}

WindowView::WindowView(BaseWindow::ptr parent, Size size, Position local_position, TextW title, unsigned title_height)
    : BaseWindow(parent, size, local_position) {}

WindowView::WindowView(BaseWindow::ptr parent, Size size, Margins margins, TextW title, unsigned title_height)
    : BaseWindow(parent, size, margins) {}

WindowView::~WindowView() {}

WindowView::ptr WindowView::create(Size size, Position position)
{
    return WindowView::ptr(new WindowView(size, position));
}

WindowView::ptr WindowView::create(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    return WindowView::ptr(new WindowView(Size(height, width), Position(start_y, start_x)));
}

WindowView::ptr WindowView::create(Size size, Position position, 
                                   TextW title, unsigned title_height)
{
    auto window = WindowView::ptr(new WindowView(size, position, title, title_height));
    window->init_title_window(title, title_height);
    return window;
}

WindowView::ptr WindowView::create(unsigned height, unsigned width, unsigned start_y, unsigned start_x, 
                                   TextW title, unsigned title_height)
{
    auto window = WindowView::ptr(new WindowView(Size(height, width), Position(start_y, start_x), title, title_height));
    window->init_title_window(title, title_height);
    return window;
}

WindowView::ptr WindowView::create(BaseWindow::ptr parent, Size size, Position local_position)
{
    if (!parent) {
        throw std::runtime_error("WindowView::create(): Parent window does not exist");
    }

    auto window = WindowView::ptr(new WindowView(parent, size, local_position));
    parent->add_child(window);
    return window;
}

WindowView::ptr WindowView::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    if (!parent) {
        throw std::runtime_error("WindowView::create(): Parent window does not exist");
    }

    auto window = WindowView::ptr(new WindowView(parent, Size(height, width), Position(start_y, start_x)));
    parent->add_child(window);
    return window;
}

WindowView::ptr WindowView::create(BaseWindow::ptr parent, Size size, Position local_position, 
                                   TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("WindowView::create(): Parent window does not exist");
    }

    auto window = WindowView::ptr(new WindowView(parent, size, local_position, title, title_height));
    window->init_title_window(title, title_height);
    parent->add_child(window);
    return window;
}

WindowView::ptr WindowView::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x, 
                                   TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("WindowView::create(): Parent window does not exist");
    }

    auto window = WindowView::ptr(new WindowView(parent, Size(height, width), Position(start_y, start_x), title, title_height));
    window->init_title_window(title, title_height);
    parent->add_child(window);
    return window;
}

WindowView::ptr WindowView::create(BaseWindow::ptr parent, Size size, Margins margins, 
                                   TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("WindowView::create(): Parent window does not exist");
    }

    auto window = WindowView::ptr(new WindowView(parent, size, margins, title, title_height));
    window->init_title_window(title, title_height);
    parent->add_child(window);
    return window;
}

WindowView::ptr WindowView::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right,
                                   TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("WindowView::create(): Parent window does not exist");
    }

    auto window = WindowView::ptr(new WindowView(parent, Size(height, width), Margins(margin_top, margin_bottom, margin_left, margin_right), title, title_height));
    window->init_title_window(title, title_height);
    parent->add_child(window);
    return window;
}

std::optional<TextW> WindowView::get_title() const
{
    if (title_window) {
        return title_window->get_title();
    }
    return std::nullopt;
}

std::optional<Size> WindowView::get_title_size() const
{
    if (title_window) {
        return title_window->get_size();
    }
    return std::nullopt;
}

unsigned WindowView::get_title_height() const
{
    if (title_window) {
        return title_window->get_height();
    }
    return 0;
}

unsigned WindowView::get_title_width() const
{
    if (title_window) {
        return title_window->get_width();
    }
    return 0;
}

void WindowView::set_title(const TextW& title)
{
    if (!title_window) {
        throw std::runtime_error("WindowView::set_title(): Title window does not exist");
    }
    title_window->set_title(title);
}

void WindowView::resize(unsigned height, unsigned width)
{
    // if (title_window.has_value()) {
    //     auto& title_window_v = title_window.value();
    //     hide_panel(title_window_v.get_panel());
    //     update_panels();

    //     if (wresize(title_window_v.get_win(), title_window_v.get_height(), width) == ERR) {
    //         throw std::runtime_error("Window::resize(): Failed to resize window");
    //     }
    // }
    BaseWindow::resize(height, width);
}

void WindowView::move_to(unsigned new_y, unsigned new_x)
{
    BaseWindow::move_to(new_y, new_x);
    // if (title_window.has_value()) {
    //     title_window.value().move_to(new_y, new_x);
    // }
}

void WindowView::update(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    if (height != size.height || width != size.width) {
        resize(height, width);
    }
    if (start_y != position.start_y || start_x != position.start_x) {
        move_to(start_y, start_x);
    }
}

void WindowView::show()
{
    if (is_showing() && !nu::display::needs_redraw(this)) {
        return;
    }

    state |= State::Showing;
    show_panel(panel.get());

    if (!active_border) {
        set_focus(is_focused());
    }
    if (active_border) {
        active_border->wdraw(win.get());
    }

    if (title_window) {
        auto title_win = title_window->get_win();
        auto title_panel = title_window->get_panel();

        show_panel(title_panel);
        if (active_border && size != title_window->get_size()) {
            active_border->wdraw(title_win);
        }

        unsigned title_window_height = title_window->get_height();
        unsigned title_window_width = title_window->get_width();
        title_window->print_title();
        // update_panels();

        if (active_border && title_window_height > 1 && title_window_height < size.height) {
            // рисуем спец. символы на стыках рамок
            wchar_t buf[2] = { L'\0', L'\0' };
            // рисуем левый стык нижней границы окна заголовка и основного окна
            buf[0] = active_border->get_border("horizontal_to_right");
            mvwaddnwstr(title_win, title_window_height-1, title_window_width-1, buf, 1);
            // рисуем правый стык нижней границы окна заголовка и основного окна
            buf[0] = active_border->get_border("horizontal_to_left");
            mvwaddnwstr(title_win, title_window_height-1, 0, buf, 1);
        }
    }

    update_panels();
    doupdate();

    nu::display::windows_manager.top_window_and_update(this);
    on_show();
}

void WindowView::hide()
{
    if (!is_showing()) {
        return;
    }
    
    state &= ~State::Showing;
    hide_panel(panel.get());
    if (title_window) {
        hide_panel(title_window->get_panel());
    }
    update_panels();
    doupdate();

    nu::display::windows_manager.hide_window_and_update(this);
    on_hide();
}

void WindowView::set_default_borders()
{
    set_unfocused_border(nu::borders::single_);
    set_focused_border(nu::borders::thick_);
}

void WindowView::set_default_event_handlers()
{
    on_hover += [this](int y, int x) {
        if (!is_focused()) {
            set_focus(true);
            nu::display::show_all_windows();
        }
    };
    on_unhover += [this]() {
        if (is_focused()) {
            set_focus(false);
            nu::display::show_all_windows();
        }
    };
}

void WindowView::init_title_window(const TextW& title, unsigned title_height)
{
    if (!title_window && title_height > 0) {
        title_window = TitleWindow::create(
            shared_from_this(),
            Size(title_height, size.width),
            Position(0, 0),
            title
        );
    }
    else if (title_window) {
        title_window->set_title(title);
    }
}

Size WindowView::calculate_size_without_title() const
{
    unsigned title_height = get_title_height();

    return Size(size.height - title_height, size.width);
}

Position WindowView::calculate_absolute_position_without_title() const
{
    unsigned title_height = get_title_height();

    return Position(position.start_y + title_height, position.start_x);
}

Position WindowView::calculate_local_position_without_title() const
{
    unsigned title_height = get_title_height();
    
    return Position(0 + title_height, 0);
}

Position WindowView::calculate_local_position_without_title_with_border() const
{
    unsigned title_height = get_title_height();
    unsigned border_length = get_border_length();

    unsigned start_y = border_length;
    unsigned start_x = border_length;
    if (title_height > 0) {
        start_y = title_height - border_length;
    }
    
    return Position(start_y, start_x);
}

Size WindowView::calculate_size_for_inner_window(Size size, Margins margins, unsigned title_height, Margins border_length)
{
    auto calc_size = BaseWindow::calculate_size_with_margins_and_borders(size, margins, border_length);
    if (title_height > 0) {
        calc_size.height -= title_height;
        calc_size.height += border_length.top;
    }
    return calc_size;
}

Position WindowView::calculate_position_for_inner_window(Position position, Margins margins, unsigned title_height, Margins border_length)
{
    auto calc_position = BaseWindow::calculate_position_with_margins_and_borders(position, margins, border_length);
    if (title_height > 0) {
        calc_position.start_y += title_height - border_length.top;
    }
    return calc_position;
}