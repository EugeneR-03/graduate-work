#include "TitleWindow.hpp"

TitleWindow::TitleWindow(Size size, Position position, TextW title)
    : BaseWindow(size, position), title(title) {}

TitleWindow::TitleWindow(BaseWindow::ptr parent, Size size, Position local_position, TextW title)
    : BaseWindow(parent, size, local_position), title(title) {}

TitleWindow::TitleWindow(BaseWindow::ptr parent, Size size, Margins margins, TextW title)
    : BaseWindow(parent, size, margins), title(title) {}

TitleWindow::~TitleWindow() {}

TitleWindow::ptr TitleWindow::create(Size size, Position position, TextW title)
{
    return TitleWindow::ptr(new TitleWindow(size, position, title));
}

TitleWindow::ptr TitleWindow::create(unsigned height, unsigned width, unsigned start_y, unsigned start_x, TextW title)
{
    return TitleWindow::ptr(new TitleWindow(Size(height, width), Position(start_y, start_x), title));
}

TitleWindow::ptr TitleWindow::create(BaseWindow::ptr parent, Size size, Position local_position, TextW title)
{
    if (!parent) {
        throw std::runtime_error("TitleWindow::TitleWindow::create(): Parent window does not exist");
    }

    auto window = TitleWindow::ptr(new TitleWindow(parent, size, local_position, title));
    parent->add_child(window);
    return window;
}

TitleWindow::ptr TitleWindow::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x, TextW title)
{
    if (!parent) {
        throw std::runtime_error("TitleWindow::TitleWindow::create(): Parent window does not exist");
    }

    auto window = TitleWindow::ptr(new TitleWindow(parent, Size(height, width), Position(start_y, start_x), title));
    parent->add_child(window);
    return window;
}

TitleWindow::ptr TitleWindow::create(BaseWindow::ptr parent, Size size, Margins margins, TextW title)
{
    if (!parent) {
        throw std::runtime_error("TitleWindow::TitleWindow::create(): Parent window does not exist");
    }

    auto window = TitleWindow::ptr(new TitleWindow(parent, size, margins, title));
    parent->add_child(window);
    return window;
}

TitleWindow::ptr TitleWindow::create(BaseWindow::ptr parent, unsigned height, unsigned width,
                        unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right, TextW title)
{
    if (!parent) {
        throw std::runtime_error("TitleWindow::TitleWindow::create(): Parent window does not exist");
    }

    auto window = TitleWindow::ptr(new TitleWindow(parent, Size(height, width), Margins(margin_top, margin_bottom, margin_left, margin_right), title));
    parent->add_child(window);
    return window;
}

TextW TitleWindow::get_title() const { return title; }

void TitleWindow::set_title(const TextW& title) { this->title = title; }

void TitleWindow::print_title()
{
    if (!win) {
        throw std::runtime_error("nu::wprint_title: win == nullptr");
    }

    unsigned border_length = get_border_length();
    int available_width = (int)size.width - 2 * border_length;
    // строка текста всего одна, поэтому height = 1
    Size text_box_size(1, available_width);
    // позиция внутри окна: по вертикали - центр, по горизонтали - сразу после левой рамки
    Position text_box_pos(size.height / 2, border_length);

    nu::wprint_text(win.get(), title, text_box_size, text_box_pos);

    // int border_length = active_border == nullptr ? 0 : 1;
    // int available_width = size.width - 2 * border_length;
    // int title_length = static_cast<int>(title.text.size());
    // // если надпись слишком длинная, усекаем по доступной длине
    // if (title_length > available_width) {
    //     title_length = available_width;
    // }

    // int y = size.height / 2;

    // int x;
    // switch (title.align) {
    //     case Align::Left:
    //     case Align::Justify:
    //         x = border_length;
    //         break;
    //     case Align::Center:
    //         x = border_length + (available_width - title_length) / 2;
    //         break;
    //     case Align::Right:
    //         x = 2 * border_length + available_width - title_length;
    //         break;
    // }

    // wattron(win, title.attrs);

    // mvwaddnwstr(win, y, x, title.text.c_str(), title_length);

    // wattroff(win, title.attrs);
}

void TitleWindow::resize(unsigned height, unsigned width)
{
    BaseWindow::resize(height, width);
}

void TitleWindow::move_to(unsigned new_y, unsigned new_x)
{
    BaseWindow::move_to(new_y, new_x);
}

void TitleWindow::update(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    BaseWindow::update(height, width, start_y, start_x);
}

void TitleWindow::show()
{
    if (is_showing() && !nu::display::needs_redraw(this)) {
        return;
    }

    state |= State::Showing;
    show_panel(panel.get());

    if (!active_border) {
        set_focus(is_focused());
    }
    active_border->wdraw(win.get());
    update_panels();

    print_title();
    update_panels();

    doupdate();

    nu::display::windows_manager.top_window_and_update(this);
    on_show();
}

void TitleWindow::hide()
{
    BaseWindow::hide();
}