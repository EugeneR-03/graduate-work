#include "FormView.hpp"

FormView::FormView(Size size, Position position)
    : WindowView(size, position) {}

FormView::FormView(Size size, Position position, TextW title, unsigned title_height)
    : WindowView(size, position) {}

FormView::FormView(BaseWindow::ptr parent, Size size, Position local_position)
    : WindowView(parent, size, local_position) {}

FormView::FormView(BaseWindow::ptr parent, Size size, Margins margins)
    : WindowView(parent, size, margins) {}

FormView::FormView(BaseWindow::ptr parent, Size size, Position local_position, TextW title, unsigned title_height)
    : WindowView(parent, size, local_position) {}

FormView::FormView(BaseWindow::ptr parent, Size size, Margins margins, TextW title, unsigned title_height)
    : WindowView(parent, size, margins) {}

FormView::ptr FormView::create(Size size, Position position)
{
    auto window = FormView::ptr(new FormView(size, position));
    window->form_window = FormSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    return window;
}

FormView::ptr FormView::create(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    auto window = FormView::ptr(new FormView(Size(height, width), Position(start_y, start_x)));
    window->form_window = FormSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    return window;
}

FormView::ptr FormView::create(Size size, Position position, 
                               TextW title, unsigned title_height)
{
    auto window = FormView::ptr(new FormView(size, position, title, title_height));
    window->init_title_window(title, title_height);
    window->form_window = FormSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    return window;
}

FormView::ptr FormView::create(unsigned height, unsigned width, unsigned start_y, unsigned start_x, 
                               TextW title, unsigned title_height)
{
    auto window = FormView::ptr(new FormView(Size(height, width), Position(start_y, start_x), title, title_height));
    window->init_title_window(title, title_height);
    window->form_window = FormSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    return window;
}

FormView::ptr FormView::create(BaseWindow::ptr parent, Size size, Position local_position)
{
    if (!parent) {
        throw std::runtime_error("FormView::create(): Parent window does not exist");
    }

    auto window = FormView::ptr(new FormView(parent, size, local_position));
    window->form_window = FormSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    parent->add_child(window);
    return window;
}

FormView::ptr FormView::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    if (!parent) {
        throw std::runtime_error("FormView::create(): Parent window does not exist");
    }

    auto window = FormView::ptr(new FormView(parent, Size(height, width), Position(start_y, start_x)));
    window->form_window = FormSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    parent->add_child(window);
    return window;
}

FormView::ptr FormView::create(BaseWindow::ptr parent, Size size, Position local_position, 
                               TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("FormView::create(): Parent window does not exist");
    }

    auto window = FormView::ptr(new FormView(parent, size, local_position, title, title_height));
    window->init_title_window(title, title_height);
    window->form_window = FormSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    parent->add_child(window);
    return window;
}

FormView::ptr FormView::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x, 
                               TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("FormView::create(): Parent window does not exist");
    }

    auto window = FormView::ptr(new FormView(parent, Size(height, width), Position(start_y, start_x), title, title_height));
    window->init_title_window(title, title_height);
    window->form_window = FormSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    parent->add_child(window);
    return window;
}

FormView::ptr FormView::create(BaseWindow::ptr parent, Size size, Margins margins, 
                               TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("FormView::create(): Parent window does not exist");
    }

    auto window = FormView::ptr(new FormView(parent, size, margins, title, title_height));
    window->init_title_window(title, title_height);
    window->form_window = FormSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    parent->add_child(window);
    return window;
}

FormView::ptr FormView::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right,
                               TextW title, unsigned title_height)
{
    if (!parent) {
        throw std::runtime_error("FormView::create(): Parent window does not exist");
    }

    auto window = FormView::ptr(new FormView(parent, Size(height, width), Margins(margin_top, margin_bottom, margin_left, margin_right), title, title_height));
    window->init_title_window(title, title_height);
    window->form_window = FormSubwindow::create(window, window->calculate_size_without_title(), window->calculate_local_position_without_title());
    parent->add_child(window);
    return window;
}

FormView::~FormView() {}

void FormView::set_form_margins(unsigned top, unsigned bottom, unsigned left, unsigned right)
{
    form_window->set_margins(top, bottom, left, right);
}

void FormView::add_field(FieldSpecification spec)
{
    form_window->add_field(spec);
}

void FormView::add_field(const std::string& label, int label_rows, int label_cols,
                         const std::string& buffer, int buffer_rows, int buffer_cols, Field_Options opts,
                         Callback callback, std::vector<wint_t> keys)
{
    form_window->add_field(label, label_rows, label_cols, buffer, buffer_rows, buffer_cols, opts, callback, keys);
}

void FormView::add_field(const std::wstring& label, int label_rows, int label_cols,
                         const std::wstring& buffer, int buffer_rows, int buffer_cols, Field_Options opts,
                         Callback callback, std::vector<wint_t> keys)
{
    form_window->add_field(label, label_rows, label_cols, buffer, buffer_rows, buffer_cols, opts, callback, keys);
}

std::string FormView::get_field_content_raw(int index)
{
    return form_window->get_field_content_raw(index);
}

std::wstring FormView::get_field_wcontent_raw(int index)
{
    return form_window->get_field_wcontent_raw(index);
}

std::string FormView::get_field_content(int index)
{
    return form_window->get_field_content(index);
}
std::wstring FormView::get_field_wcontent(int index)
{
    return form_window->get_field_wcontent(index);
}

void FormView::set_field_content(int index, const std::string& content)
{
    form_window->set_field_content(index, content);
}

void FormView::set_field_wcontent(int index, const std::wstring& wcontent)
{
    form_window->set_field_wcontent(index, wcontent);
}

void FormView::add_exit_callback(Callback callback, std::vector<wint_t> keys)
{
    form_window->add_exit_callback(callback, keys);
}

void FormView::resize(unsigned height, unsigned width)
{
    WindowView::resize(height, width);
}

void FormView::move_to(unsigned new_y, unsigned new_x)
{
    WindowView::move_to(new_y, new_x);
}

void FormView::update(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    if (height != size.height || width != size.width) {
        resize(height, width);
    }
    if (start_y != position.start_y || start_x != position.start_x) {
        move_to(start_y, start_x);
    }
}

void FormView::show()
{
    WindowView::show();
    form_window->show();
}

void FormView::hide()
{
    WindowView::hide();
    form_window->stop();
    form_window->hide();
}

void FormView::update_form()
{
    form_window->update_form();
}

void FormView::run()
{
    form_window->run();
}

void FormView::stop()
{
    form_window->stop();
}

bool FormView::is_running()
{
    return form_window->is_running();
}

void FormView::set_default_mouse_event_handlers()
{
    form_window->set_default_mouse_event_handlers();
}

void FormView::set_listeners_for_inner_window_events()
{
    form_window->on_resize += [this]() {
        on_resize();
    };
    form_window->on_move += [this]() {
        on_move();
    };
    form_window->on_show += [this]() {
        on_show();
    };
    form_window->on_hide += [this]() {
        on_hide();
    };
    form_window->on_idle += [this]() {
        on_idle();
    };
    form_window->on_focus += [this]() {
        on_focus();
    };
    form_window->on_unfocus += [this]() {
        on_unfocus();
    };
    form_window->on_field_text_changed += [this]() {
        on_field_text_changed();
    };
}