#include "ButtonView.hpp"

ButtonView::ButtonView(Size size, Position position, TextW title)
    : WindowView(size, position, title, size.height)
{
    set_default_borders();
}

ButtonView::ButtonView(BaseWindow::ptr parent, Size size, Position local_position, TextW title)
    : WindowView(parent, size, local_position, title, size.height)
{
    set_default_borders();
}

ButtonView::ButtonView(BaseWindow::ptr parent, Size size, Margins margins, TextW title)
    : WindowView(parent, size, margins, title, size.height)
{
    set_default_borders();
}

ButtonView::ptr ButtonView::create(Size size, Position position, TextW title)
{
    auto window = ButtonView::ptr(new ButtonView(size, position, title));
    window->init_title_window(title, size.height);
    return window;
}

ButtonView::ptr ButtonView::create(unsigned height, unsigned width, unsigned start_y, unsigned start_x, TextW title)
{
    auto window = ButtonView::ptr(new ButtonView(Size(height, width), Position(start_y, start_x), title));
    window->init_title_window(title, height);
    return window;
}

ButtonView::ptr ButtonView::create(BaseWindow::ptr parent, Size size, Position local_position, TextW title)
{
    if (!parent) {
        throw std::runtime_error("ButtonView::create(): Parent window does not exist");
    }
    auto window = ButtonView::ptr(new ButtonView(parent, size, local_position, title));
    window->init_title_window(title, size.height);
    parent->add_child(window);
    return window;
}

ButtonView::ptr ButtonView::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x, TextW title)
{
    if (!parent) {
        throw std::runtime_error("ButtonView::create(): Parent window does not exist");
    }
    auto window = ButtonView::ptr(new ButtonView(parent, Size(height, width), Position(start_y, start_x), title));
    window->init_title_window(title, height);
    parent->add_child(window);
    return window;
}

ButtonView::ptr ButtonView::create(BaseWindow::ptr parent, Size size, Margins margins, TextW title)
{
    if (!parent) {
        throw std::runtime_error("ButtonView::create(): Parent window does not exist");
    }
    auto window = ButtonView::ptr(new ButtonView(parent, size, margins, title));
    window->init_title_window(title, size.height);
    parent->add_child(window);
    return window;
}

ButtonView::ptr ButtonView::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right, TextW title)
{
    if (!parent) {
        throw std::runtime_error("ButtonView::create(): Parent window does not exist");
    }
    auto window = ButtonView::ptr(new ButtonView(parent, Size(height, width), Margins(margin_top, margin_bottom, margin_left, margin_right), title));
    window->init_title_window(title, height);
    parent->add_child(window);
    return window;
}

ButtonView::~ButtonView() {}

TextW ButtonView::get_text()
{
    if (!title_window) {
        throw std::runtime_error("ButtonView::get_text(): Title window does not exist");
    }
    return title_window->get_title();
}

void ButtonView::set_text(TextW text)
{
    if (!title_window) {
        throw std::runtime_error("ButtonView::set_text(): Title window does not exist");
    }
    title_window->set_title(text);
}