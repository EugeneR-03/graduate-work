#include "ModalView.hpp"

ModalView::ModalView(Size size, Position position,
                     TextW title, unsigned title_height,
                     TextW message,
                     ButtonView::ptr ok_button, Position ok_button_position)
    : WindowView(size, position, title, title_height),
      text(message)
{
    this->ok_button = ok_button;
    this->cancel_button = nullptr;
}

ModalView::ModalView(Size size, Position position, 
                     TextW title, unsigned title_height, 
                     TextW message, 
                     ButtonView::ptr ok_button, Position ok_button_position, 
                     ButtonView::ptr cancel_button, Position cancel_button_position)
    : WindowView(size, position, title, title_height),
      text(message)
{
    this->ok_button = ok_button;
    this->cancel_button = cancel_button;
}

ModalView::~ModalView()
{
    // удаляем кнопки из детей
    if (ok_button.get()) {
        remove_child(ok_button.get());
    }
    if (cancel_button.get()) {
        remove_child(cancel_button.get());
    }
}

ModalView::ptr ModalView::create_default(TextW title, TextW message, TextW ok_text, unsigned start_y, unsigned start_x)
{
    auto ok_button = ButtonView::create(3, 14, 0, 0, ok_text);
    auto ok_button_position = Position(8, 8);

    auto modal_view = ModalView::ptr(new ModalView(
        Size(12, 30), 
        Position(start_y, start_x), 
        title, 3, 
        message, 
        ok_button, ok_button_position
    ));
    modal_view->init_title_window(title, 3);
    modal_view->add_child(ok_button);
    modal_view->set_unfocused_border(nu::borders::single_);
    modal_view->set_focused_border(nu::borders::single_);

    modal_view->get_ok_button()->set_parent(
        modal_view, 
        ok_button_position.start_y, 
        ok_button_position.start_x
    );

    modal_view->on_hide += [modal_view, ok_button]() {
        if (ok_button->is_focused()) {
            ok_button->set_focus(false);
        }
        modal_view->focused_button = nullptr;
    };

    ok_button->on_hover += [ok_button](int y, int x) {
        if (!ok_button->is_focused()) {
            ok_button->set_focus(true);
            nu::display::show_all_windows();
        }
    };
    ok_button->on_unhover += [modal_view, ok_button]() {
        if (modal_view->focused_button == ok_button.get()) {
            return;
        }
        if (ok_button->is_focused()) {
            ok_button->set_focus(false);
            nu::display::show_all_windows();
        }
    };
    ok_button->on_left_click += [modal_view](int y, int x) {
        modal_view->answer = true;
        modal_view->stop();
    };

    return modal_view;
}

ModalView::ptr ModalView::create_default(TextW title, TextW message, TextW ok_text, TextW cancel_text, unsigned start_y, unsigned start_x)
{
    auto ok_button = ButtonView::create(3, 12, 0, 0, ok_text);
    auto ok_button_position = Position(8, 2);

    auto cancel_button = ButtonView::create(3, 12, 0, 0, cancel_text);
    auto cancel_button_position = Position(8, 16);

    auto modal_view = ModalView::ptr(new ModalView(
        Size(12, 30), 
        Position(start_y, start_x), 
        title, 3, 
        message, 
        ok_button, ok_button_position, 
        cancel_button, cancel_button_position
    ));
    modal_view->init_title_window(title, 3);
    modal_view->add_child(ok_button);
    modal_view->add_child(cancel_button);
    modal_view->set_unfocused_border(nu::borders::single_);
    modal_view->set_focused_border(nu::borders::single_);

    modal_view->get_ok_button()->set_parent(
        modal_view, 
        ok_button_position.start_y, 
        ok_button_position.start_x
    );
    modal_view->get_cancel_button()->set_parent(
        modal_view, 
        cancel_button_position.start_y, 
        cancel_button_position.start_x
    );

    modal_view->on_hide += [modal_view, ok_button, cancel_button]() {
        ok_button->set_focus(false);
        cancel_button->set_focus(false);
        modal_view->focused_button = nullptr;
    };

    ok_button->on_hover += [ok_button](int y, int x) {
        if (!ok_button->is_focused()) {
            ok_button->set_focus(true);
            nu::display::show_all_windows();
        }
    };
    ok_button->on_unhover += [modal_view, ok_button]() {
        if (modal_view->focused_button == ok_button.get()) {
            return;
        }
        if (ok_button->is_focused()) {
            ok_button->set_focus(false);
            nu::display::show_all_windows();
        }
    };
    ok_button->on_left_click += [modal_view](int y, int x) {
        modal_view->answer = true;
        modal_view->stop();
    };
    
    cancel_button->on_hover += [cancel_button](int y, int x) {
        if (!cancel_button->is_focused()) {
            cancel_button->set_focus(true);
            nu::display::show_all_windows();
        }
    };
    cancel_button->on_unhover += [modal_view, cancel_button]() {
        if (modal_view->focused_button == cancel_button.get()) {
            return;
        }
        if (cancel_button->is_focused()) {
            cancel_button->set_focus(false);
            nu::display::show_all_windows();
        }
    };
    cancel_button->on_left_click += [modal_view](int y, int x) {
        modal_view->answer = false;
        modal_view->stop();
    };

    return modal_view;
}

Size ModalView::get_default_size()
{
    return Size(12, 30);
}

void ModalView::set_ok_button(ButtonView::ptr button, Position position)
{
    ok_button = button;
    ok_button->set_parent(shared_from_this(), position.start_y, position.start_x);
}

ButtonView* ModalView::get_ok_button()
{
    return ok_button.get();
}

void ModalView::set_cancel_button(ButtonView::ptr button, Position position)
{
    cancel_button = button;
    cancel_button->set_parent(shared_from_this(), position.start_y, position.start_x);
}

ButtonView* ModalView::get_cancel_button()
{
    return cancel_button.get();
}

void ModalView::set_text(TextW text)
{
    this->text = text;
}

TextW ModalView::get_text()
{
    return text;
}

void ModalView::set_ok_text(TextW text)
{
    if (ok_button) {
        ok_button->set_text(text);
    }
}

TextW ModalView::get_ok_text()
{
    if (ok_button) {
        return ok_button->get_text();
    }
    return TextW();
}

void ModalView::set_cancel_text(TextW text)
{
    if (cancel_button) {
        cancel_button->set_text(text);
    }
}

TextW ModalView::get_cancel_text()
{
    if (cancel_button) {
        return cancel_button->get_text();
    }
    return TextW();
}

void ModalView::resize(unsigned height, unsigned width)
{
    WindowView::resize(height, width);
}

void ModalView::move_to(unsigned new_y, unsigned new_x)
{
    WindowView::move_to(new_y, new_x);
}

void ModalView::update(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    WindowView::update(height, width, start_y, start_x);
}

void ModalView::show()
{
    WindowView::show();
    if (ok_button) {
        ok_button->show();
    }
    if (cancel_button) {
        cancel_button->show();
    }
    print_message();
}

void ModalView::hide()
{
    if (ok_button) {
        ok_button->hide();
    }
    if (cancel_button) {
        cancel_button->hide();
    }
    WindowView::hide();
}

void ModalView::run()
{
    if (!win) {
        throw std::runtime_error("ModalView::run: win == nullptr");
    }

    is_running_ = true;
    while (is_running_) {
        int ch = nu::wget_wch(win.get(), nu::work_mode);

        is_running_ = custom_menu_driver(ch);
    }

    stop();
    hide();
}

void ModalView::stop()
{
    is_running_ = false;
    nu::display::windows_manager.reset_running_window();
}

bool ModalView::is_running()
{
    return is_running_;
}

void ModalView::print_message()
{
    auto title_height = get_title_height();
    auto border_length = get_border_length();
    auto calc_size = WindowView::calculate_size_for_inner_window(size, get_margins(), title_height, Margins(border_length));
    auto calc_position = WindowView::calculate_position_for_inner_window(Position(0, 0), get_margins(), title_height, Margins(border_length));

    nu::wprint_text(get_win(), text, calc_size, calc_position);

    update_panels();
    doupdate();
}

bool ModalView::custom_menu_driver(int ch)
{
    switch (ch) {
        case KEY_LEFT:
            if (ok_button && !cancel_button) {
                focused_button = ok_button.get();
                ok_button->set_focus(true);
            }
            else if (cancel_button && !ok_button) {
                focused_button = cancel_button.get();
                cancel_button->set_focus(true);
            }
            else if (ok_button && cancel_button) {
                focused_button = ok_button.get();
                ok_button->set_focus(true);
                cancel_button->set_focus(false);
            }
            nu::display::show_all_windows();
            return true;
        
        case KEY_RIGHT:
            if (ok_button && !cancel_button) {
                focused_button = ok_button.get();
                ok_button->set_focus(true);
            }
            else if (cancel_button && !ok_button) {
                focused_button = cancel_button.get();
                cancel_button->set_focus(true);
            }
            else if (ok_button && cancel_button) {
                focused_button = cancel_button.get();
                cancel_button->set_focus(true);
                ok_button->set_focus(false);
            }
            nu::display::show_all_windows();
            return true;
        
        case KEY_ENTER:
            if (!focused_button) {
                if (ok_button && !cancel_button) {
                    focused_button = ok_button.get();
                    // ok_button->set_focus(true);
                    // nu::display::show_all_windows();
                }
                else if (cancel_button && !ok_button) {
                    focused_button = cancel_button.get();
                    // cancel_button->set_focus(true);
                    // nu::display::show_all_windows();
                }
                // return true;
            }
            if (focused_button == ok_button.get()) {
                answer = true;
            }
            else {
                answer = false;
            }
            focused_button->on_left_click(0, 0);
            return false;
        
        case BUTTON1_CLICKED:
            return is_running_;
            
        case K_ESCAPE:
            return false;
        
        default:
            return true;
    }
}