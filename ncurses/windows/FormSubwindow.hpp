#pragma once

#include <ncursesw/ncurses.h>
#include <ncursesw/panel.h>
#include <ncursesw/form.h>

#include "IRunnable.hpp"
#include "BaseWindow.hpp"

/* TODO:
    - добавить поддержку динамических полей (однострочных и многострочных)
    - добавить поддержку многострочных описаний
*/

struct FieldSpecification {
    std::wstring label;
    chtype label_attrs;
    std::wstring buffer;
    chtype buffer_attrs;
    std::wstring buffer_init_text;
    Field_Options opts;
    int label_height;
    int label_width;
    int label_start_y;
    int label_start_x;
    int buffer_height;
    int buffer_width;
    int buffer_start_y;
    int buffer_start_x;
    int offscreen_rows;
    int offset_x;
    int nbuf;
};

class FormSubwindow : public BaseWindow, public IRunnable {
public:
    using ptr = std::shared_ptr<FormSubwindow>;
    
    static FormSubwindow::ptr create(BaseWindow::ptr parent, Size size, Position local_position);
    static FormSubwindow::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x);
    static FormSubwindow::ptr create(BaseWindow::ptr parent, Size size, Margins margins);
    static FormSubwindow::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right);
    
    FormSubwindow(const FormSubwindow& other) = delete;
    FormSubwindow& operator=(const FormSubwindow& other) = delete;
    FormSubwindow(FormSubwindow&& other) = delete;
    FormSubwindow& operator=(FormSubwindow&& other) = delete;
    ~FormSubwindow() override;

    void set_unfocused_border(std::shared_ptr<IBorder> unfocused_border) = delete;
    void set_focused_border(std::shared_ptr<IBorder> inactive_border) = delete;
    void set_active_border(std::shared_ptr<IBorder> active_border) = delete;

    void add_field(FieldSpecification spec);
    void add_field(const std::string& label, int label_rows, int label_cols,
                   const std::string& buffer, int buffer_rows, int buffer_cols, Field_Options opts = 0,
                   Callback callback = nullptr, std::vector<wint_t> keys = {});
    void add_field(const std::wstring& label, int label_rows, int label_cols,
                   const std::wstring& buffer, int buffer_rows, int buffer_cols, Field_Options opts = 0,
                   Callback callback = nullptr, std::vector<wint_t> keys = {});
    std::string get_field_content_raw(int index);
    std::wstring get_field_wcontent_raw(int index);
    std::string get_field_content(int index);
    std::wstring get_field_wcontent(int index);
    void set_field_content(int index, const std::string& content);
    void set_field_wcontent(int index, const std::wstring& content);

    void add_exit_callback(Callback callback, std::vector<wint_t> keys);
    
    void resize(unsigned height, unsigned width) override;
    void move_to(unsigned new_y, unsigned new_x) override;
    void update(unsigned height, unsigned width, unsigned start_y, unsigned start_x) override;
    void show() override;
    void hide() override;
    void update_form();

    void run() override;
    void stop() override;
    bool is_running() override;

    void set_default_mouse_event_handlers();

    Event<void()> on_field_text_changed;

protected:
    FORM_ptr form;

    std::vector<FIELD*> fields;
    std::vector<FieldSpecification> fields_specifications;

    std::vector<KeysCallback> form_callbacks;
    std::vector<KeysCallback> exit_callbacks;

    FormSubwindow(BaseWindow::ptr parent, Size size, Position local_position);
    FormSubwindow(BaseWindow::ptr parent, Size size, Margins margins);

    void on_left_click_handler(int y, int x);
    void on_left_press_handler(int y, int x);
    void on_left_release_handler(int y, int x);

    void show_field(int field_index);
    void post_custom_form();
    void unpost_custom_form();
    bool execute_exit_callback(wint_t wch);
    bool execute_form_callback(int index, wint_t wch);
    bool is_field_buffer_empty(int index);
    int calculate_chars_to_move_for_switching_fields(int cur_x, int old_field_index, int new_field_index);
    unsigned calculate_pos();
    bool custom_form_driver(wint_t wch);
    void update_field(int index, int chars_to_move, bool is_resetting_current_field = false);
    void update_fields(int chars_to_move = 0, bool is_resetting_current_field = false);

private:
    bool is_running_ = false;
};