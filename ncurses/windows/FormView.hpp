#pragma once

#include "IRunnable.hpp"
#include "BaseWindow.hpp"
#include "FormSubwindow.hpp"
#include "WindowView.hpp"

class FormView : public WindowView, public IRunnable {
public:
    using ptr = std::shared_ptr<FormView>;

    static FormView::ptr create(Size size, Position position);
    static FormView::ptr create(unsigned height, unsigned width, unsigned start_y, unsigned start_x);
    static FormView::ptr create(Size size, Position position, 
                                TextW title, unsigned title_height);
    static FormView::ptr create(unsigned height, unsigned width, unsigned start_y, unsigned start_x, 
                                TextW title, unsigned title_height);
    static FormView::ptr create(BaseWindow::ptr parent, Size size, Position local_position = Position(0, 0));
    static FormView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y = 0, unsigned start_x = 0);
    static FormView::ptr create(BaseWindow::ptr parent, Size size, Margins margins = Margins(0, 0, 0, 0));
    static FormView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top = 0, unsigned margin_bottom = 0, unsigned margin_left = 0, unsigned margin_right = 0);
    static FormView::ptr create(BaseWindow::ptr parent, Size size, Position local_position, 
                                TextW title, unsigned title_height);
    static FormView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x, 
                                TextW title, unsigned title_height);
    static FormView::ptr create(BaseWindow::ptr parent, Size size, Margins margins, 
                                TextW title, unsigned title_height);
    static FormView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, 
                                unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right, 
                                TextW title, unsigned title_height);

    FormView(const FormView& other) = delete;
    FormView& operator=(const FormView& other) = delete;
    FormView(FormView&& other) = delete;
    FormView& operator=(FormView&& other) = delete;
    ~FormView() override;

    void set_form_margins(unsigned top, unsigned bottom, unsigned left, unsigned right);

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
    FormSubwindow::ptr form_window;

    FormView(Size size, Position position);
    FormView(Size size, Position position, TextW title, unsigned title_height);
    FormView(BaseWindow::ptr parent, Size size, Position local_position);
    FormView(BaseWindow::ptr parent, Size size, Margins margins);
    FormView(BaseWindow::ptr parent, Size size, Position local_position, TextW title, unsigned title_height);
    FormView(BaseWindow::ptr parent, Size size, Margins margins, TextW title, unsigned title_height);

    void set_listeners_for_inner_window_events();
};