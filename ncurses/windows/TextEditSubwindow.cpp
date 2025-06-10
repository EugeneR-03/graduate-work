#include "TextEditSubwindow.hpp"

TextEditSubwindow::TextEditSubwindow(BaseWindow::ptr parent, Size size, Position position)
    : BaseWindow(parent, size, position) 
{
    lines.emplace_back(L"");
    line_number_width = 1;
    keypad(win.get(), true);
}

TextEditSubwindow::TextEditSubwindow(BaseWindow::ptr parent, Size size, Margins margins)
    : BaseWindow(parent, size, margins)
{
    lines.emplace_back(L"");
    line_number_width = 1;
    keypad(win.get(), true);
}

TextEditSubwindow::~TextEditSubwindow() {}

TextEditSubwindow::ptr TextEditSubwindow::create(BaseWindow::ptr parent, Size size, Position position)
{
    if (!parent) {
        throw std::runtime_error("TextEditSubwindow::create(): Parent window does not exist");
    }
    auto widget = TextEditSubwindow::ptr(new TextEditSubwindow(parent, size, position));
    parent->add_child(widget);
    return widget;
}
TextEditSubwindow::ptr TextEditSubwindow::create(BaseWindow::ptr parent, unsigned height, unsigned width,
                                                unsigned start_y, unsigned start_x)
{
    return create(parent, Size(height, width), Position(start_y, start_x));
}
TextEditSubwindow::ptr TextEditSubwindow::create(BaseWindow::ptr parent, Size size, Margins margins)
{
    if (!parent) {
        throw std::runtime_error("TextEditSubwindow::create(): Parent window does not exist");
    }
    auto widget = TextEditSubwindow::ptr(new TextEditSubwindow(parent, size, margins));
    parent->add_child(widget);
    return widget;
}
TextEditSubwindow::ptr TextEditSubwindow::create(BaseWindow::ptr parent, unsigned height, unsigned width,
                                                unsigned margin_top, unsigned margin_bottom,
                                                unsigned margin_left, unsigned margin_right)
{
    return create(parent, Size(height, width), 
                  Margins(margin_top, margin_bottom, margin_left, margin_right));
}

void TextEditSubwindow::load(const std::vector<std::string>& input_lines)
{
    lines.clear();
    lines.reserve(input_lines.size());
    std::transform(input_lines.begin(), input_lines.end(), std::back_inserter(lines), nu::string_to_wstring);

    if (lines.empty()) {
        lines.emplace_back(L"");
    }

    line_number_width = std::to_wstring(lines.size()).length();
    // сбрасываем курсор и прокрутку к началу.
    cursor_line = cursor_col = 0;
    first_visible_line = first_visible_col = 0;
}

std::vector<std::string> TextEditSubwindow::get_lines() const
{
    std::vector<std::string> output;
    output.reserve(lines.size());
    std::transform(lines.begin(), lines.end(), std::back_inserter(output), nu::wstring_to_string);
    return output;
}

void TextEditSubwindow::resize(unsigned height, unsigned width)
{
    BaseWindow::resize(height, width);
}

void TextEditSubwindow::move_to(unsigned new_y, unsigned new_x)
{
    BaseWindow::move_to(new_y, new_x);
}

void TextEditSubwindow::update(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    BaseWindow::update(height, width, start_y, start_x);
}

void TextEditSubwindow::show()
{
    if (is_showing() && !nu::display::needs_redraw(this)) {
        return;
    }

    state |= State::Showing;
    show_panel(panel.get());
    draw_content();
    update_panels();
    doupdate();
    
    nu::display::windows_manager.top_window_and_update(this);
    on_show();
}

void TextEditSubwindow::hide()
{
    state &= ~State::Showing;
    hide_panel(panel.get());
    stop();
    update_panels();
    doupdate();
    
    nu::display::windows_manager.hide_window_and_update(this);
    on_hide();
}

void TextEditSubwindow::ensure_cursor_visible()
{
    unsigned visible_rows = size.height - horizontal_scrollbar_height_;
    if (cursor_line < first_visible_line) {
        first_visible_line = cursor_line;
    }
    else if (cursor_line >= first_visible_line + visible_rows) {
        first_visible_line = cursor_line - visible_rows + 1;
    }

    unsigned usable_width = size.width - vertical_scrollbar_width_;
    unsigned text_start_x = line_number_width + delimiter.length();
    if (text_start_x > usable_width) {
        text_start_x = usable_width;
    }

    unsigned content_width = 
        usable_width > text_start_x 
        ? usable_width - text_start_x 
        : 0;
    
    if (cursor_col < first_visible_col) {
        first_visible_col = cursor_col;
    }
    else if (cursor_col >= first_visible_col + content_width) {
        if (content_width > 0) {
            first_visible_col = cursor_col - content_width + 1;
        }
        else {
            first_visible_col = cursor_col;
        }
    }
}

void TextEditSubwindow::pos_cursor()
{
    unsigned screen_y = cursor_line - first_visible_line;
    unsigned screen_x = (cursor_col - first_visible_col) + line_number_width + delimiter.length();

    if (screen_y < size.height && screen_x < size.width) {
        wmove(win.get(), screen_y, screen_x);
    }
}

void TextEditSubwindow::insert_char(wchar_t ch)
{
    std::wstring& line = lines[cursor_line];
    line.insert(cursor_col, 1, ch);
    cursor_col += 1;  // перемещаем курсор вправо
}

void TextEditSubwindow::backspace_char()
{
    if (cursor_col > 0) {
        std::wstring& line = lines[cursor_line];
        line.erase(cursor_col - 1, 1);
        cursor_col -= 1;
    }
    else if (cursor_line > 0) {
        // если в начале строки, то соединяем с предыдущей строкой
        std::wstring prev_line = lines[cursor_line - 1];
        std::wstring curr_line = lines[cursor_line];
        // перемещаем курсор в конец предыдущей строки.
        cursor_line -= 1;
        cursor_col = prev_line.size();
        // объединяем строки
        lines[cursor_line] = prev_line + curr_line;
        // удаляем текущую строку
        lines.erase(lines.begin() + (cursor_line + 1));
        line_number_width = std::to_wstring(lines.size()).length();
    }
}

void TextEditSubwindow::delete_char()
{
    std::wstring& line = lines[cursor_line];
    if (cursor_col < line.size()) {
        line.erase(cursor_col, 1);
    }
    else {
        // в конце строки: соединяем со следующей строкой, если она есть
        if (cursor_line + 1 < lines.size()) {
            std::wstring next_line = lines[cursor_line + 1];
            // объединяем текущую и следующую строку
            lines[cursor_line] = line + next_line;
            // удаляем следующую строку
            lines.erase(lines.begin() + (cursor_line + 1));
            line_number_width = std::to_wstring(lines.size()).length();
        }
    }
}

void TextEditSubwindow::break_line()
{
    std::wstring& line = lines[cursor_line];
    std::wstring left_part  = line.substr(0, cursor_col);
    std::wstring right_part = line.substr(cursor_col);
    // текущая строка становится левой частью.
    lines[cursor_line] = left_part;
    // новая строка с правой частью вставляется следом.
    lines.insert(lines.begin() + cursor_line + 1, right_part);
    // перемещаем курсор в начало новой строки.
    cursor_line += 1;
    cursor_col = 0;
    line_number_width = std::to_wstring(lines.size()).length();
}

bool TextEditSubwindow::handle_key(wint_t wch, bool is_editable)
{
    int rc = E_OK;
    switch (wch) {
        case KEY_UP:
            if (cursor_line > 0) {
                cursor_line -= 1;
                // если новая строка короче текущего столбца
                if (cursor_col > lines[cursor_line].size()) {
                    cursor_col = lines[cursor_line].size();
                }
            }
            else {
                // перемещаем в начало строки
                cursor_col = 0;
            }
            break;

        case KEY_DOWN:
            if (cursor_line + 1 < lines.size()) {
                cursor_line += 1;
                if (cursor_col > lines[cursor_line].size()) {
                    cursor_col = lines[cursor_line].size();
                }
            }
            else if (cursor_line + 1 == lines.size()) {
                // перемещаем в конец строки
                cursor_col = lines[cursor_line].size();
            }
            break;

        case KEY_LEFT:
            if (cursor_col > 0) {
                cursor_col -= 1;
            }
            else if (cursor_line > 0) {
                cursor_line -= 1;
                cursor_col = lines[cursor_line].size();
            }
            break;

        case KEY_RIGHT:
            if (cursor_col < lines[cursor_line].size()) {
                cursor_col += 1;
            }
            else if (cursor_line + 1 < lines.size()) {
                cursor_line += 1;
                cursor_col = 0;
            }
            break;

        case KEY_HOME:
            cursor_col = 0;
            break;

        case KEY_END:
            cursor_col = lines[cursor_line].size();
            break;

        case KEY_PPAGE:
            if (cursor_line > 0) {
                unsigned page = (size.height > 1 ? size.height - 1 : 1);
                cursor_line = (cursor_line < page ? 0 : cursor_line - page);
                if (cursor_col > lines[cursor_line].size()) {
                    cursor_col = lines[cursor_line].size();
                }
            }
            break;

        case KEY_NPAGE:
            if (cursor_line < lines.size() - 1) {
                unsigned page = (size.height > 1 ? size.height - 1 : 1);
                cursor_line = std::min((unsigned)lines.size() - 1, cursor_line + page);
                if (cursor_col > lines[cursor_line].size()) {
                    cursor_col = lines[cursor_line].size();
                }
            }
            break;

        case 10:
        case KEY_ENTER:
            if (!is_editable) {
                rc = E_REQUEST_DENIED;
                break;
            }
            break_line();
            break;

        case KEY_BACKSPACE:
            if (!is_editable) {
                rc = E_REQUEST_DENIED;
                break;
            }
            backspace_char();
            break;

        case KEY_DC:
            if (!is_editable) {
                rc = E_REQUEST_DENIED;
                break;
            }
            delete_char();
            break;

        default:
            if (!is_editable) {
                rc = E_REQUEST_DENIED;
                break;
            }
            if (iswprint(wch)) {
                insert_char(wch);
            }
            else {
                rc = E_REQUEST_DENIED;
            }
            break;
    }

    if (rc == E_REQUEST_DENIED) {
        return false;
    }

    ensure_cursor_visible();
    draw_content();
    return true;
}

void TextEditSubwindow::set_vertical_scrollbar(bool value)
{
    need_vertical_scrollbar_ = value;
    if (need_vertical_scrollbar_) {
        vertical_scrollbar_width_ = 1;
    }
    else {
        vertical_scrollbar_width_ = 0;
    }
}

void TextEditSubwindow::draw_vertical_scrollbar()
{
    unsigned usable_width = size.width - vertical_scrollbar_width_;
    int total_rows = lines.size();
    int rows_visible = size.height - horizontal_scrollbar_height_;
    if (total_rows > rows_visible) {
        int max_scroll = total_rows - rows_visible;
        float ratio = (float)rows_visible / (float)total_rows;
        int thumb_h = std::max(1, (int)(rows_visible * ratio)); // высота ползунка
        int track_space = rows_visible - thumb_h;
        int top_index = first_visible_line;
        // расположение верхней границы ползунка
        int thumb_y;
        if (top_index <= 0) {
            thumb_y = 0;
        }
        else if (top_index >= max_scroll) {
            thumb_y = track_space;
        }
        else {
            thumb_y = 1 + ((top_index - 1) * (track_space - 1)) / (max_scroll - 1);
        }
        int thumb_x = usable_width;
        // линия прокрутки
        for (int i = 0; i < rows_visible; i++) {
            mvwaddch(win.get(), i, thumb_x, ACS_VLINE);
        }
        // ползунок
        for (int i = 0; i < thumb_h; i++) {
            mvwaddwstr(win.get(), thumb_y + i, thumb_x, L"█");
        }
    }
}

void TextEditSubwindow::set_horizontal_scrollbar(bool value)
{
    need_horizontal_scrollbar_ = value;
    if (need_horizontal_scrollbar_) {
        horizontal_scrollbar_height_ = 1;
    }
    else {
        horizontal_scrollbar_height_ = 0;
    }
}

void TextEditSubwindow::draw_horizontal_scrollbar()
{
    unsigned usable_w = size.width - vertical_scrollbar_width_;
    unsigned track_x = line_number_width + delimiter.length();
    if (track_x >= usable_w) {
        return;
    }

    // ширина линии прокрутки
    unsigned track_w = usable_w - track_x;

    // длина самого длинного текста
    unsigned max_len = 0;
    for (auto &l : lines) max_len = std::max<unsigned>(max_len, l.size());
    if (max_len <= track_w) {
        return;
    }

    // размер ползунка
    unsigned thumb_w = std::max(1u, (unsigned)((float)track_w * ((float)track_w / max_len)));
    unsigned track_sp = track_w - thumb_w;

    // смещение ползунка
    unsigned max_scroll = max_len - track_w;
    unsigned thumb_x;
    if (first_visible_col == 0) {
        thumb_x = 0;
    }
    else if (first_visible_col >= max_scroll) {
        thumb_x = track_sp;
    }
    else {
        thumb_x = 1 + ((first_visible_col - 1) * (track_sp - 1)) / (max_scroll - 1);
    }

    unsigned y = size.height - 1;

    // линия прокрутки
    for (unsigned i = 0; i < track_w; ++i) {
        mvwaddch(win.get(), y, track_x + i, ACS_HLINE);
    }

    // ползунок
    for (unsigned i = 0; i < thumb_w; ++i) {
        mvwaddwstr(win.get(), y, track_x + thumb_x + i, L"━");
    }
}

void TextEditSubwindow::draw_content()
{
    update_panels();
    werase(win.get());

    // сколько строк можно вывести, если будет горизонтальный скроллбар
    bool need_horizontal_scrollbar =
        ([](const std::vector<std::wstring>& l, unsigned w) {
            unsigned mx = 0; for (auto& s : l) mx = std::max<unsigned>(mx, s.size());
            return mx > w;
        })(lines, size.width - 1);
    set_horizontal_scrollbar(need_horizontal_scrollbar);
    unsigned rows_for_text = size.height - horizontal_scrollbar_height_;

    bool need_vertical_scrollbar = (lines.size() > rows_for_text);
    set_vertical_scrollbar(need_vertical_scrollbar);
    
    unsigned usable_width = size.width - vertical_scrollbar_width_;
    unsigned text_start_x = line_number_width + delimiter.length();
    if (text_start_x > usable_width) text_start_x = usable_width;
    unsigned text_width = (usable_width > text_start_x ? usable_width - text_start_x : 0);

    for (unsigned screen_y = 0; screen_y < rows_for_text; screen_y++) {
        unsigned line_index = first_visible_line + screen_y;
        if (line_index >= lines.size()) {
            break;  // нет больше строк для отображения
        }
        // формируем строку с номером и разделителем.
        std::wstring num_str = std::to_wstring(line_index + 1);
        // дополняем пробелами слева до ширины номера.
        if (num_str.length() < line_number_width) {
            num_str.insert(num_str.begin(), line_number_width - num_str.length(), L' ');
        }
        num_str += delimiter;
        // вычисляем фрагмент строки текста, попадающий в видимую область по горизонтали.
        const std::wstring& line = lines[line_index];
        std::wstring visible_text;
        if (first_visible_col < line.size()) {
            visible_text = line.substr(first_visible_col, text_width);
        }
        else {
            visible_text = L""; // вся строка прокручена вправо, ничего не видно
        }
        mvwaddnwstr(win.get(), screen_y, 0, num_str.c_str(), num_str.length());     // номер строки и разделитель
        mvwaddnwstr(win.get(), screen_y, text_start_x, visible_text.c_str(), visible_text.length());    // текст строки
    }

    if (need_vertical_scrollbar_) {
        draw_vertical_scrollbar();
    }
    if (need_horizontal_scrollbar_) {
        draw_horizontal_scrollbar();
    }

    pos_cursor();
    update_panels();
    doupdate();
}

void TextEditSubwindow::run()
{
    if (!win) {
        throw std::runtime_error("FormWindow::run(): form_window is nullptr");
    }
    if (nu::display::set_running_window(this) == ERR) {
        return;
    }
    
    curs_set(1);
    draw_content();
    is_running_ = true;
    while (is_running_) {
        if (!is_showing()) {
            return;
        }

        wint_t wch = nu::wget_wch(win.get(), nu::work_mode);
        if (execute_exit_callback(wch) || wch == K_ESCAPE) {
            set_focus(false);
            is_running_ = false;
            nu::display::show_all_windows();
            break;
        }
        handle_key(wch);
    }

    if (is_running_) {
        stop();
    }
}

void TextEditSubwindow::run_non_editable()
{
    if (!win) {
        throw std::runtime_error("FormWindow::run(): form_window is nullptr");
    }
    if (nu::display::set_running_window(this) == ERR) {
        return;
    }
    
    curs_set(1);
    draw_content();
    is_running_ = true;
    while (is_running_) {
        if (!is_showing()) {
            return;
        }

        wint_t wch = nu::wget_wch(win.get(), nu::work_mode);
        if (execute_exit_callback(wch) || wch == K_ESCAPE) {
            set_focus(false);
            is_running_ = false;
            nu::display::show_all_windows();
            break;
        }
        handle_key(wch, false);
    }

    if (is_running_) {
        stop();
    }
}

void TextEditSubwindow::stop()
{
    if (is_running_) {
        nu::display::reset_running_window();
    }
    is_running_ = false;
    if (is_showing()) {
        set_focus(false);
        curs_set(0);
    }
}

bool TextEditSubwindow::is_running() {
    return is_running_;
}

void TextEditSubwindow::add_exit_callback(Callback callback, std::vector<wint_t> keys)
{
    if (callback) {
        auto pair = std::make_pair(keys, callback);
        exit_callbacks.push_back(pair);
    }
}

bool TextEditSubwindow::execute_exit_callback(wint_t ch)
{
    if (exit_callbacks.empty()) {
        return false;
    }

    bool found = false;
    for (auto& pair : exit_callbacks) {
        auto& keys = pair.first;
        auto& callback = pair.second;
        
        for (auto key : keys) {
            if (key == ch) {
                if (callback) {
                    callback();
                    found = true;
                }
                break;
            }
        }
    }
    return found;
}