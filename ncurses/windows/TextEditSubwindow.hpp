#pragma once

#include "IRunnable.hpp"
#include "BaseWindow.hpp"
#include "./../NcursesUtils.hpp"

class TextEditSubwindow : public BaseWindow, IRunnable {
public:
    using ptr = std::shared_ptr<TextEditSubwindow>;

    static ptr create(BaseWindow::ptr parent, Size size, Position position);
    static ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, 
                      unsigned start_y, unsigned start_x);
    static ptr create(BaseWindow::ptr parent, Size size, Margins margins);
    static ptr create(BaseWindow::ptr parent, unsigned height, unsigned width,
                      unsigned margin_top, unsigned margin_bottom, 
                      unsigned margin_left, unsigned margin_right);

    TextEditSubwindow(const TextEditSubwindow& other) = delete;
    TextEditSubwindow& operator=(const TextEditSubwindow& other) = delete;
    TextEditSubwindow(TextEditSubwindow&& other) = delete;
    TextEditSubwindow& operator=(TextEditSubwindow&& other) = delete;
    ~TextEditSubwindow() override;

    void set_unfocused_border(std::shared_ptr<IBorder> unfocused_border) = delete;
    void set_focused_border(std::shared_ptr<IBorder> inactive_border) = delete;
    void set_active_border(std::shared_ptr<IBorder> active_border) = delete;

    void load(const std::vector<std::string>& lines);
    std::vector<std::string> get_lines() const;

    void resize(unsigned height, unsigned width) override;
    void move_to(unsigned new_y, unsigned new_x) override;
    void update(unsigned height, unsigned width, unsigned start_y, unsigned start_x) override;
    void show() override;
    void hide() override;

    void run() override;
    void run_non_editable();
    void stop() override;
    bool is_running() override;

    void add_exit_callback(Callback callback = nullptr, std::vector<wint_t> keys = {});

    Event<void()> on_cursor_moved;
    Event<void()> on_text_changed;

protected:
    std::vector<std::wstring> lines;  // текстовый буфер (каждый элемент - строка)
    unsigned cursor_line = 0;         // текущая позиция курсора (номер строки)
    unsigned cursor_col  = 0;         // текущая позиция курсора (номер столбца в строке)
    unsigned first_visible_line = 0;  // индекс первой видимой строки (вертикальная прокрутка)
    unsigned first_visible_col  = 0;  // индекс первого видимого символа (горизонтальная прокрутка)
    unsigned line_number_width  = 1;  // ширина поля для номера строки (кол-во цифр макс. номера)
    std::wstring delimiter = L" | ";
    std::vector<KeysCallback> exit_callbacks;

    TextEditSubwindow(BaseWindow::ptr parent, Size size, Position local_position);
    TextEditSubwindow(BaseWindow::ptr parent, Size size, Margins margins);

    void insert_char(wchar_t ch);
    void backspace_char();
    void delete_char();
    void break_line();
    bool handle_key(wint_t wch, bool is_editable = true);

    void set_vertical_scrollbar(bool value);
    void draw_vertical_scrollbar();
    void set_horizontal_scrollbar(bool value);
    void draw_horizontal_scrollbar();
    
    void draw_content();

    void ensure_cursor_visible();
    void pos_cursor();

    bool execute_exit_callback(wint_t wch);

private:
    bool need_vertical_scrollbar_ = false;
    unsigned vertical_scrollbar_width_ = 0;
    bool need_horizontal_scrollbar_ = false;
    unsigned horizontal_scrollbar_height_ = 0;
    bool is_running_ = false;
};