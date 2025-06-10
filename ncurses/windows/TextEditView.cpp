#include "TextEditView.hpp"

TextEditView::TextEditView(Size size, Position position)
    : WindowView(size, position) {}

TextEditView::TextEditView(Size size, Position position, TextW title, unsigned title_height)
    : WindowView(size, position) {}

TextEditView::TextEditView(BaseWindow::ptr parent, Size size, Position position)
    : WindowView(parent, size, position) {}

TextEditView::TextEditView(BaseWindow::ptr parent, Size size, Margins margins)
    : WindowView(parent, size, margins) {}

TextEditView::TextEditView(BaseWindow::ptr parent, Size size, Position position, TextW title, unsigned title_height)
    : WindowView(parent, size, position) {}

TextEditView::TextEditView(BaseWindow::ptr parent, Size size, Margins margins, TextW title, unsigned title_height)
    : WindowView(parent, size, margins) {}

TextEditView::ptr TextEditView::create(Size size, Position position)
{
    return TextEditView::ptr(new TextEditView(size, position));
}

TextEditView::ptr TextEditView::create(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    return create(Size(height, width), Position(start_y, start_x));
}

TextEditView::ptr TextEditView::create(Size size, Position position, TextW title, unsigned title_height)
{
    auto window = TextEditView::ptr(new TextEditView(size, position, title, title_height));
    window->init_title_window(title, title_height);
    window->text_window = TextEditSubwindow::create(
        window, 
        window->calculate_size_without_title(), 
        window->calculate_local_position_without_title()
    );
    return window;
}

TextEditView::ptr TextEditView::create(unsigned height, unsigned width, unsigned start_y, unsigned start_x,
                                       TextW title, unsigned title_height)
{
    auto window = TextEditView::ptr(new TextEditView(Size(height, width), Position(start_y, start_x), title, title_height));
    window->init_title_window(title, title_height);
    window->text_window = TextEditSubwindow::create(
        window, 
        window->calculate_size_without_title(), 
        window->calculate_local_position_without_title()
    );
    return window;
}

TextEditView::ptr TextEditView::create(BaseWindow::ptr parent, Size size, Position local_position)
{
    if (!parent) {
        throw std::runtime_error("TextEditView::create(): Parent window does not exist");
    }
    auto window = TextEditView::ptr(new TextEditView(parent, size, local_position));
    window->text_window = TextEditSubwindow::create(window, size, Position(0, 0));
    parent->add_child(window);
    return window;
}

TextEditView::ptr TextEditView::create(BaseWindow::ptr parent, unsigned height, unsigned width,
                                       unsigned start_y, unsigned start_x)
{
    return create(parent, Size(height, width), Position(start_y, start_x));
}

TextEditView::ptr TextEditView::create(BaseWindow::ptr parent, Size size, Position local_position,
                                       TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("TextEditView::create(): Parent window does not exist");
    }
    auto window = TextEditView::ptr(new TextEditView(parent, size, local_position, title, title_height));
    window->init_title_window(title, title_height);
    window->text_window = TextEditSubwindow::create(
        window, 
        window->calculate_size_without_title(), 
        window->calculate_local_position_without_title()
    );
    parent->add_child(window);
    return window;
}

TextEditView::ptr TextEditView::create(BaseWindow::ptr parent, unsigned height, unsigned width,
                                       unsigned start_y, unsigned start_x, TextW title, unsigned title_height)
{
    return create(parent, Size(height, width), Position(start_y, start_x), title, title_height);
}

TextEditView::ptr TextEditView::create(BaseWindow::ptr parent, Size size, Margins margins)
{
    if (!parent) {
        throw std::runtime_error("TextEditView::create(): Parent window does not exist");
    }
    auto window = TextEditView::ptr(new TextEditView(parent, size, margins));
    window->text_window = TextEditSubwindow::create(
        window, window->calculate_size_without_title(),
        window->calculate_local_position_without_title()
    );
    parent->add_child(window);
    return window;
}

TextEditView::ptr TextEditView::create(BaseWindow::ptr parent, Size size, Margins margins,
                                       TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("TextEditView::create(): Parent window does not exist");
    }
    auto window = TextEditView::ptr(new TextEditView(parent, size, margins, title, title_height));
    window->init_title_window(title, title_height);
    window->text_window = TextEditSubwindow::create(
        window,
        window->calculate_size_without_title(),
        window->calculate_local_position_without_title()
    );
    parent->add_child(window);
    return window;
}

TextEditView::ptr TextEditView::create(BaseWindow::ptr parent, unsigned height, unsigned width,
                                       unsigned margin_top, unsigned margin_bottom,
                                       unsigned margin_left, unsigned margin_right,
                                       TextW title, unsigned title_height)
{
    return create(
        parent, Size(height, width), 
        Margins(margin_top, margin_bottom, margin_left, margin_right),
        title, title_height
    );
}

TextEditView::~TextEditView() {}

void TextEditView::load(const std::vector<std::string>& lines)
{
    if (text_window) {
        text_window->load(lines);
    }
}

std::vector<std::string> TextEditView::get_lines() const
{
    return text_window ? text_window->get_lines() : std::vector<std::string>();
}

void TextEditView::resize(unsigned height, unsigned width)
{
    WindowView::resize(height, width);
}

void TextEditView::move_to(unsigned start_y, unsigned start_x)
{
    WindowView::move_to(start_y, start_x);
}

void TextEditView::update(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    if (height != size.height || width != size.width) {
        resize(height, width);
    }
    if (start_y != position.start_y || start_x != position.start_x) {
        move_to(start_y, start_x);
    }
}

void TextEditView::show()
{
    WindowView::show();
    text_window->show();
}

void TextEditView::hide()
{
    WindowView::hide();
    text_window->stop();
    text_window->hide();
}

void TextEditView::run()
{
    text_window->run();
}

void TextEditView::run_non_editable()
{
    text_window->run_non_editable();
}

void TextEditView::stop()
{
    text_window->stop();
}

bool TextEditView::is_running()
{
    return text_window->is_running();
}

void TextEditView::add_exit_callback(Callback callback, std::vector<wint_t> keys)
{
    text_window->add_exit_callback(callback, keys);
}