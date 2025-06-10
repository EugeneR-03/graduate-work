#pragma once

#include "IRunnable.hpp"
#include "WindowView.hpp"
#include "ButtonView.hpp"
#include "./../NcursesUtils.hpp"

class ModalView : public WindowView, public IRunnable {
public:
    using ptr = std::shared_ptr<ModalView>;

    static ModalView::ptr create_default(TextW title, TextW message, TextW ok_text, unsigned start_y, unsigned start_x);
    static ModalView::ptr create_default(TextW title, TextW message, TextW ok_text, TextW cancel_text, unsigned start_y, unsigned start_x);
    static Size get_default_size();

    ModalView(ModalView& other) = delete;
    ModalView& operator=(const ModalView& other) = delete;
    ModalView(ModalView&& other) = delete;
    ModalView& operator=(ModalView&& other) = delete;
    ~ModalView();

    void set_ok_button(ButtonView::ptr button, Position position);
    ButtonView* get_ok_button();
    void set_cancel_button(ButtonView::ptr button, Position position);
    ButtonView* get_cancel_button();
    void set_text(TextW text);
    TextW get_text();
    void set_ok_text(TextW text);
    TextW get_ok_text();
    void set_cancel_text(TextW text);
    TextW get_cancel_text();

    void resize(unsigned height, unsigned width) override;
    void move_to(unsigned new_y, unsigned new_x) override;
    void update(unsigned height, unsigned width, unsigned start_y, unsigned start_x) override;
    void show() override;
    void hide() override;

    void run() override;
    void stop() override;
    bool is_running() override;

protected:
    ButtonView::ptr ok_button;
    ButtonView::ptr cancel_button;
    TextW text;
    ButtonView* focused_button = nullptr;
    bool is_running_ = false;
    bool answer = false;

    ModalView(Size size, Position position, TextW title, unsigned title_height, TextW message, ButtonView::ptr ok_button, Position ok_button_position);
    ModalView(Size size, Position position, TextW title, unsigned title_height, TextW message, ButtonView::ptr ok_button, Position ok_button_position, ButtonView::ptr cancel_button, Position cancel_button_position);

    void print_message();
    bool custom_menu_driver(int ch);
};