#include "FormSubwindow.hpp"

FormSubwindow::FormSubwindow(BaseWindow::ptr parent, Size size, Position local_position)
    : BaseWindow(parent, size, local_position),
      form(nullptr),
      fields(),
      fields_specifications(),
      form_callbacks(),
      exit_callbacks() {}

FormSubwindow::FormSubwindow(BaseWindow::ptr parent, Size size, Margins margins)
    : BaseWindow(parent, size, margins),
      form(nullptr),
      fields(),
      fields_specifications(),
      form_callbacks(),
      exit_callbacks() {}

FormSubwindow::ptr FormSubwindow::create(BaseWindow::ptr parent, Size size, Position local_position)
{
    if (!parent) {
        throw std::runtime_error("FormSubwindow::create(): Parent window does not exist");
    }
    auto window = FormSubwindow::ptr(new FormSubwindow(parent, size, local_position));
    parent->add_child(window);
    return window;
}

FormSubwindow::ptr FormSubwindow::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    if (!parent) {
        throw std::runtime_error("FormSubwindow::create(): Parent window does not exist");
    }
    auto window = FormSubwindow::ptr(new FormSubwindow(parent, Size(height, width), Position(start_y, start_x)));
    parent->add_child(window);
    return window;
}

FormSubwindow::ptr FormSubwindow::create(BaseWindow::ptr parent, Size size, Margins margins)
{
    if (!parent) {
        throw std::runtime_error("FormSubwindow::create(): Parent window does not exist");
    }
    auto window = FormSubwindow::ptr(new FormSubwindow(parent, size, margins));
    parent->add_child(window);
    return window;
}

FormSubwindow::ptr FormSubwindow::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right)
{
    if (!parent) {
        throw std::runtime_error("FormSubwindow::create(): Parent window does not exist");
    }
    auto window = FormSubwindow::ptr(new FormSubwindow(parent, Size(height, width), Margins(margin_top, margin_bottom, margin_left, margin_right)));
    parent->add_child(window);
    return window;
}

FormSubwindow::~FormSubwindow()
{
    if (form) {
        form.reset();
    }
}

void FormSubwindow::add_field(FieldSpecification spec)
{
    fields_specifications.push_back(spec);
    
    FIELD* field = new_field(spec.buffer_height, spec.buffer_width, spec.buffer_start_y, spec.buffer_start_x, spec.offscreen_rows, spec.nbuf);
    if (!field) {
        throw std::runtime_error("FormWindow::add_field(): Can't create field");
    }

    field_opts_on(field, O_EDIT | O_VISIBLE | O_PUBLIC | O_ACTIVE | O_STATIC);
    Field_Options opts;
    opts = spec.opts;
    if (spec.buffer_height > 1) {
        opts |= O_WRAP;
    }
    field_opts_on(field, opts);
    field_opts_off(field, O_AUTOSKIP | O_BLANK);
    // if (spec.buffer_height > 1) {
    //     field_opts_off(field, O_STATIC);
    //     set_max_field(field, spec.buffer_height);
    // }
    if (!spec.buffer_init_text.empty()) {
        set_field_buffer(field, 0, nu::wstring_to_char(spec.buffer_init_text));
    }
    set_field_pad(field, '_');

    if (fields.empty()) {
        fields.push_back(nullptr);
    }
    fields.insert(--fields.end(), field);
}

void FormSubwindow::add_field(const std::string& label, int label_rows, int label_cols,
                              const std::string& buffer, int buffer_rows, int buffer_cols, Field_Options opts,
                              std::function<void()> callback, std::vector<wint_t> keys)
{
    add_field(nu::string_to_wstring(label), label_rows, label_cols, 
              nu::string_to_wstring(buffer), buffer_rows, buffer_cols, opts, 
              callback, keys);
}

void FormSubwindow::add_field(const std::wstring& label, int label_rows, int label_cols,
                              const std::wstring& buffer, int buffer_rows, int buffer_cols, Field_Options opts,
                              std::function<void()> callback, std::vector<wint_t> keys)
{
    int y_offset = 0;
    for (const auto& prev_field : fields_specifications) {
        int h = std::max(prev_field.label_height, prev_field.buffer_height);
        y_offset += h;
    }
    FieldSpecification spec;
    spec.label = label;
    spec.label_height = label_rows;
    spec.label_width = label_cols;
    spec.label_start_y = 0 + y_offset;
    spec.label_start_x = 0;
    spec.label_attrs = A_NORMAL;
    spec.buffer = L"";
    spec.buffer_height = buffer_rows;
    spec.buffer_width = buffer_cols;
    spec.buffer_start_y = 0 + y_offset;
    spec.buffer_start_x = 0 + label_cols;
    spec.buffer_init_text = buffer;
    spec.buffer_attrs = A_NORMAL;
    spec.opts = opts;
    spec.offscreen_rows = 0;
    spec.offset_x = 0;
    spec.nbuf = 0;

    add_field(spec);
    if (callback) {
        auto pair = std::make_pair(keys, callback);
        form_callbacks.push_back(pair);
    }
    else {
        auto pair = std::make_pair(std::vector<wint_t>(), nullptr);
        form_callbacks.push_back(pair);
    }
}

std::string FormSubwindow::get_field_content_raw(int index)
{
    if ((size_t)index >= fields.size() - 1) {
        throw std::out_of_range("FormWindow::get_field_content: index out of range");
    }
    char* buf = field_buffer(fields[index], 0);
    auto& field_spec = fields_specifications[index];
    if (!buf) {
        return "";
    }
    // обрезаем массив
    std::string temp(buf, field_spec.buffer_width);
    return temp;
}

std::wstring FormSubwindow::get_field_wcontent_raw(int index)
{
    return nu::string_to_wstring(get_field_content_raw(index));
}

std::string FormSubwindow::get_field_content(int index)
{
    return nu::wstring_to_string(get_field_wcontent(index));
}

std::wstring FormSubwindow::get_field_wcontent(int index)
{
    if ((size_t)index >= fields.size() - 1) {
        throw std::out_of_range("FormWindow::get_field_content: index out of range");
    }
    const auto& field_spec = fields_specifications[index];
    return field_spec.buffer;
}

void FormSubwindow::set_field_content(int index, const std::string& content)
{
    set_field_wcontent(index, nu::string_to_wstring(content));
}

void FormSubwindow::set_field_wcontent(int index, const std::wstring& wcontent)
{
    if (index < 0 || index >= (int)fields.size() - 1) {
        throw std::out_of_range("FormWindow::set_field_content(): index out of range");
    }
    auto& field_spec = fields_specifications[index];
    field_spec.buffer = wcontent;
    if (form) {
        update_fields();
        update_panels();
        doupdate();
    }
}

void FormSubwindow::add_exit_callback(std::function<void()> callback, std::vector<wint_t> keys)
{
    if (callback) {
        auto pair = std::make_pair(keys, callback);
        exit_callbacks.push_back(pair);
    }
}

void FormSubwindow::resize(unsigned height, unsigned width)
{
    if (form) {
        unpost_custom_form();
    }
    BaseWindow::resize(height, width);
    if (form) {
        post_custom_form();
    }
}

void FormSubwindow::move_to(unsigned new_y, unsigned new_x)
{
    if (form) {
        unpost_custom_form();
    }
    BaseWindow::move_to(new_y, new_x);
    if (form) {
        post_custom_form();
    }
}

void FormSubwindow::update(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    if (form) {
        unpost_custom_form();
    }
    BaseWindow::update(height, width, start_y, start_x);
    if (form) {
        post_custom_form();
    }
}

void FormSubwindow::show()
{
    if (is_showing() && !nu::display::needs_redraw(this)) {
        return;
    }

    state |= State::Showing;
    show_panel(panel.get());
    update_form();
    doupdate();
    
    nu::display::windows_manager.top_window_and_update(this);
    on_show();
}

void FormSubwindow::hide()
{
    state &= ~State::Showing;
    unpost_custom_form();
    hide_panel(panel.get());

    stop();
    
    update_panels();
    doupdate();
    
    nu::display::windows_manager.hide_window_and_update(this);
    on_hide();
}

void FormSubwindow::update_form()
{
    if (form) {
        form.reset();
    }
    if (panel) {
        panel.reset();
    }
    if (win) {
        win.reset();
    }
    
    if (fields.empty()) {
        return;
    }

    win = WINDOW_ptr(derwin(
        get_parent()->get_win(), 
        size.height, size.width, 
        position.start_y, position.start_x
    ));
    if (!win) {
        throw std::runtime_error("FormWindow::update_form(): Failed to create window");
    }
    panel = PANEL_ptr(new_panel(win.get()));
    if (!panel) {
        throw std::runtime_error("FormWindow::update_form(): Failed to create panel");
    }
    
    form = FORM_ptr(new_form(fields.data()));
    if (!form) {
        throw std::runtime_error("FormWindow::update_form(): Failed to create form");
    }

    set_form_win(form.get(), get_parent()->get_win());
    set_form_sub(form.get(), win.get());
    post_custom_form();

    update_panels();
}

void FormSubwindow::run()
{
    if (!win) {
        throw std::runtime_error("FormWindow::run(): form_window is nullptr");
    }
    if (nu::display::set_running_window(this) == ERR) {
        return;
    }

    // включаем курсор
    curs_set(1);
    form_driver(form.get(), REQ_FIRST_FIELD);
    form_driver(form.get(), REQ_BEG_LINE);
    pos_form_cursor(form.get());
    update_fields();
    update_panels();

    if (!is_focused()) {
        set_focus(true);
    }
    doupdate();

    is_running_ = true;
    while (is_running_) {
        if (!is_showing()) {
            return;
        }

        wint_t wch = nu::wget_wch(win.get(), nu::work_mode);

        if (execute_exit_callback(wch) || wch == K_ESCAPE) {
            break;
        }
        custom_form_driver(wch);

        update_panels();
        doupdate();
    }

    if (is_running_) {
        stop();
    }
}

void FormSubwindow::stop()
{
    if (is_running_) {
        nu::display::reset_running_window();
    }
    is_running_ = false;
    if (is_showing()) {
        set_focus(false);
        // выключаем курсор
        curs_set(0);
    }
}

bool FormSubwindow::is_running()
{
    return is_running_;
}

void FormSubwindow::set_default_mouse_event_handlers()
{
    on_left_click += [this](int y, int x) { this->on_left_click_handler(y, x); };
}

void FormSubwindow::on_left_click_handler(int y, int x)
{
    if (!is_running_) {
        run();
        return;
    }

    int win_y, win_x;
    getbegyx(win.get(), win_y, win_x);
    int local_y = y - win_y;
    int local_x = x - win_x;

    for (size_t i = 0; i < fields_specifications.size(); ++i) {
        auto& spec = fields_specifications[i];
        // попали в область буфера?
        if (local_y == spec.buffer_start_y
            && local_x >= spec.buffer_start_x
            && local_x <  spec.buffer_start_x + spec.buffer_width)
        {
            // переводим в позицию в строке
            int col = local_x - spec.buffer_start_x + spec.offset_x;
            col = std::clamp(col, 0, (int)spec.buffer.length());

            // меняем активное поле
            set_current_field(form.get(), fields[i]);

            // двигаем курсор внутри формы
            // (update_field — ваша функция, двигающая курсор в буфере)
            update_field(i, col, /*is_resetting=*/false);
            pos_form_cursor(form.get());
            return;
        }
    }
}

void FormSubwindow::on_left_press_handler(int y, int x)
{

}

void FormSubwindow::on_left_release_handler(int y, int x)
{

}

void FormSubwindow::show_field(int field_index)
{
    auto field = fields[field_index];
    if (!field) {
        return;
    }
    const auto& field_spec = fields_specifications[field_index];

    if (!field_spec.label.empty()) {
        wattron(win.get(), field_spec.label_attrs);

        auto label_to_show = field_spec.label;
        if (label_to_show.length() > (size_t)field_spec.label_width) {
            label_to_show = label_to_show.substr(0, field_spec.label_width);
        }
        mvwaddwstr(win.get(), field_spec.label_start_y, field_spec.label_start_x, label_to_show.c_str());

        wattroff(win.get(), field_spec.label_attrs);
    }

    update_panels();
}

void FormSubwindow::post_custom_form()
{
    if (fields.empty() || fields.size() == 1) {
        return;
    }
    if (!form) {
        throw std::runtime_error("FormWindow::post_custom_form(): form is nullptr");
    }

    set_current_field(form.get(), fields[0]);
    int rc = post_form(form.get());
    if (rc == E_NO_ROOM) {
        throw std::runtime_error("FormWindow::post_custom_form(): no room for form");
    }

    form_driver(form.get(), REQ_FIRST_FIELD);
    form_driver(form.get(), REQ_BEG_LINE);
    pos_form_cursor(form.get());

    // wrefresh(form_window);
    for (size_t i = 0; i < fields.size() - 1; i++) {
        show_field(i);
    }
}

void FormSubwindow::unpost_custom_form()
{
    if (!form) {
        return;
    }
    unpost_form(form.get());
}

bool FormSubwindow::execute_exit_callback(wint_t wch)
{
    if (exit_callbacks.empty()) {
        return false;
    }

    bool found = false;
    for (auto& pair : exit_callbacks) {
        auto& keys = pair.first;
        auto& callback = pair.second;
        
        for (auto key : keys) {
            if (key == wch) {
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

bool FormSubwindow::execute_form_callback(int index, wint_t wch)
{
    const auto& pair = form_callbacks[index];
    const auto& keys = pair.first;
    const auto& callback = pair.second;

    bool found = false;
    for (auto key : keys) {
        if (key == wch) {
            if (callback) {
                callback();
                found = true;
            }
            break;
        }
    }

    return found;
}

bool FormSubwindow::is_field_buffer_empty(int index)
{
    if ((size_t)index >= fields_specifications.size()) {
        throw std::out_of_range("FormWindow::check_field_buffer_for_text: index out of range");
    }
    auto& field_spec = fields_specifications[index];
    if (field_spec.buffer.empty()) {
        return true;
    }
    return false;
}

int FormSubwindow::calculate_chars_to_move_for_switching_fields(int cur_x, int old_field_index, int new_field_index)
{
    const auto& old_field_spec = fields_specifications[old_field_index];
    const auto& new_field_spec = fields_specifications[new_field_index];
    int chars_to_move;
    if ((int)new_field_spec.buffer.length() < cur_x - old_field_spec.buffer_start_x) {
        chars_to_move = new_field_spec.buffer.length();
    }
    else {
        chars_to_move = cur_x - old_field_spec.buffer_start_x;
    }
    return chars_to_move;
}

unsigned FormSubwindow::calculate_pos()
{
    [[maybe_unused]] int cur_y;
    int cur_x;
    getyx(win.get(), cur_y, cur_x);
    auto cur_spec = fields_specifications[field_index(current_field(form.get()))];
    int row = cur_y - cur_spec.buffer_start_y;
    int col = cur_x - cur_spec.buffer_start_x;
    int filled_rows_chars_count = row * cur_spec.buffer_width;
    return filled_rows_chars_count + col;
}

bool FormSubwindow::custom_form_driver(wint_t wch)
{
    int cur_index = field_index(current_field(form.get()));
    if (cur_index < 0) {
        throw std::runtime_error("FormWindow::custom_form_driver(): no items in form");
    }

    // обрабатываем коллбэки
    if (execute_form_callback(cur_index, wch)) {
        return true;
    }

    auto& cur_spec = fields_specifications[cur_index];
    [[maybe_unused]] int cur_y;
    int cur_x;
    getyx(win.get(), cur_y, cur_x);

    int row = cur_y - cur_spec.buffer_start_y;
    int col = cur_x - cur_spec.buffer_start_x;
    int filled_rows_chars_count = row * cur_spec.buffer_width;
    int pos = filled_rows_chars_count + col;

    int field_content_length = get_field_wcontent(cur_index).length();
    int end_x = cur_spec.buffer_start_x + field_content_length - filled_rows_chars_count;
    
    int last_row_to_fill = field_content_length / cur_spec.buffer_width;
    
    // передаем сигнал оригинальному драйверу
    int new_index = -1;
    int rc = E_REQUEST_DENIED;
    switch (wch) {
        case KEY_UP: {
            rc = form_driver_w(form.get(), KEY_CODE_YES, REQ_PREV_FIELD);
            new_index = field_index(current_field(form.get()));
            if (is_field_buffer_empty(new_index)) {
                break;
            }

            int chars_to_move = calculate_chars_to_move_for_switching_fields(cur_x, cur_index, field_index(current_field(form.get())));
            for (int i = 0; i < chars_to_move; i++) {
                form_driver(form.get(), REQ_RIGHT_CHAR);
            }
            break;
        }

        case (wint_t)L'\t':
        case KEY_ENTER:
        case KEY_DOWN: {
            rc = form_driver_w(form.get(), KEY_CODE_YES, REQ_NEXT_FIELD);
            new_index = field_index(current_field(form.get()));
            if (is_field_buffer_empty(new_index)) {
                break;
            }

            int chars_to_move = calculate_chars_to_move_for_switching_fields(cur_x, cur_index, field_index(current_field(form.get())));
            for (int i = 0; i < chars_to_move; i++) {
                form_driver(form.get(), REQ_RIGHT_CHAR);
            }
            break;
        }

        case KEY_LEFT: {
            if (is_field_buffer_empty(cur_index)) {
                break;
            }

            rc = form_driver_w(form.get(), KEY_CODE_YES, REQ_PREV_CHAR);
            break;
        }

        case KEY_RIGHT: {
            if (is_field_buffer_empty(cur_index)) {
                break;
            }

            if (filled_rows_chars_count == 0) {
                if (cur_x >= end_x) {
                    rc = E_REQUEST_DENIED;
                    break;
                }
            }
            else {
                if (cur_x >= end_x && row == last_row_to_fill) {
                    rc = E_REQUEST_DENIED;
                    break;
                }
            }
            rc = form_driver_w(form.get(), KEY_CODE_YES, REQ_NEXT_CHAR);
            break;
        }

        case KEY_BACKSPACE: {
            if (is_field_buffer_empty(cur_index) || pos == 0) {
                break;
            }

            rc = form_driver_w(form.get(), KEY_CODE_YES, REQ_DEL_PREV);
            cur_spec.buffer.erase(pos - 1, 1);
            update_field(cur_index, pos - 1);
            break;
        }

        case KEY_DC: {
            if (is_field_buffer_empty(cur_index)) {
                break;
            }

            rc = form_driver_w(form.get(), KEY_CODE_YES, REQ_DEL_CHAR);
            cur_spec.buffer.erase(pos, 1);
            update_field(cur_index, pos);
            break;
        }

        default: {
            if (iswprint(wch)) {
                if (is_field_buffer_empty(cur_index)) {
                    update_fields(true);
                }

                int max_chars = cur_spec.buffer_width * cur_spec.buffer_height;
                if ((int)cur_spec.buffer.length() >= max_chars) {
                    rc = E_REQUEST_DENIED;
                    break;
                }

                cur_spec.buffer.insert(pos, 1, wch);
                if (Flags::has_flag(cur_spec.opts, O_PASSWORD)) {
                    rc = form_driver_w(form.get(), 0, '*');
                }
                else {
                    rc = form_driver_w(form.get(), 0, wch);
                }
                
                form_driver_w(form.get(), KEY_CODE_YES, REQ_VALIDATION);
                int chars_to_move = pos + 1;
                update_field(cur_index, chars_to_move);
                on_field_text_changed();
            }
            else {
                rc = E_REQUEST_DENIED;
            }
            break;
        }
    }

    // если сигнал не опознан,
    // возвращаем управление в main-loop
    if (rc == E_REQUEST_DENIED) {
        return false;
    }

    pos_form_cursor(form.get());
    return true;
}

void FormSubwindow::update_field(int index, int chars_to_move, bool is_resetting_current_field)
{
    const auto& field_spec = fields_specifications[index];
    int active_field_index = field_index(current_field(form.get()));

    if (index == active_field_index) {
        if (is_field_buffer_empty(index)) {
            if (is_resetting_current_field) {
                set_field_buffer(fields[index], 0, "");
            }
            else {
                set_field_buffer(fields[index], 0, nu::wstring_to_char(field_spec.buffer_init_text));
            }
        }
        else {
            if (Flags::has_flag(field_spec.opts, O_PASSWORD)) {
                std::string password(field_spec.buffer.size(), ' ');
                for (int j = 0; j < (int)field_spec.buffer.size(); j++) {
                    if (!isspace(field_spec.buffer[j])) {
                        password[j] = '*';
                    }
                }
                set_field_buffer(fields[index], 0, password.c_str());
            }
            else {
                set_field_buffer(fields[index], 0, nu::wstring_to_char(field_spec.buffer));
            }
        }
        // переходим к началу поля
        form_driver(form.get(), REQ_BEG_FIELD);
        // REQ_BEG_FIELD переходит к первому символу, игнорируя пробелы в начале поля
        // поэтому вручную переходим в самое начало
        int pos = calculate_pos();
        for (int i = 0; i < pos; i ++) {
            form_driver(form.get(), REQ_LEFT_CHAR);
        }
        for (int i = 0; i < chars_to_move; i++) {
            form_driver(form.get(), REQ_NEXT_CHAR);
        }
        return;
    }

    if (is_field_buffer_empty(index)) {
        set_field_buffer(fields[index], 0, nu::wstring_to_char(field_spec.buffer_init_text));
    }
    else {
        if (Flags::has_flag(field_spec.opts, O_PASSWORD)) {
            std::string password(field_spec.buffer.size(), ' ');
            for (int j = 0; j < (int)field_spec.buffer.size(); j++) {
                if (!isspace(field_spec.buffer[j])) {
                    password[j] = '*';
                }
            }
            set_field_buffer(fields[index], 0, password.c_str());
        }
        else {
            set_field_buffer(fields[index], 0, nu::wstring_to_char(field_spec.buffer));
        }
    }
}

void FormSubwindow::update_fields(int chars_to_move, bool is_resetting_current_field)
{
    for (size_t i = 0; i < fields.size() - 1; i++) {
        update_field(i, chars_to_move, is_resetting_current_field);
    }
}