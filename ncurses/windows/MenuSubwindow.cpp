#include "MenuSubwindow.hpp"

MenuSubwindow::MenuSubwindow(BaseWindow::ptr parent, Size size, Position local_position)
    : BaseWindow(parent, size, local_position),
      menu(nullptr),
      menu_mark(L""),
      menu_mark_char(nullptr),
      menu_items_names(),
      menu_items_descs(),
      menu_items(),
      menu_callbacks(), 
      exit_callbacks() {}

MenuSubwindow::MenuSubwindow(BaseWindow::ptr parent, Size size, Margins margins)
    : BaseWindow(parent, size, margins),
      menu(nullptr),
      menu_mark(L""),
      menu_mark_char(nullptr),
      menu_items_names(),
      menu_items_descs(),
      menu_items(),
      menu_callbacks(),
      exit_callbacks() {}

MenuSubwindow::~MenuSubwindow()
{
    for (auto name : menu_items_names) {
        free(name);
    }
    for (auto desc : menu_items_descs) {
        free(desc);
    }
    if (menu_mark_char) {
        free(menu_mark_char);
    }

    if (menu_items.size() > 0) {
        for (auto item : menu_items) {
            free_item(item);
        }
        menu_items.clear();
    }

    if (menu) {
        menu.reset();
    }
}

MenuSubwindow::ptr MenuSubwindow::create(BaseWindow::ptr parent, Size size, Position local_position)
{
    if (!parent) {
        throw std::runtime_error("MenuSubwindow::create(): Parent window does not exist");
    }
    auto window = MenuSubwindow::ptr(new MenuSubwindow(parent, size, local_position));
    parent->add_child(window);
    return window;
}

MenuSubwindow::ptr MenuSubwindow::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    if (!parent) {
        throw std::runtime_error("MenuSubwindow::create(): Parent window does not exist");
    }
    auto window = MenuSubwindow::ptr(new MenuSubwindow(parent, Size(height, width), Position(start_y, start_x)));
    parent->add_child(window);
    return window;
}

MenuSubwindow::ptr MenuSubwindow::create(BaseWindow::ptr parent, Size size, Margins margins)
{
    if (!parent) {
        throw std::runtime_error("MenuSubwindow::create(): Parent window does not exist");
    }
    auto window = MenuSubwindow::ptr(new MenuSubwindow(parent, size, margins));
    parent->add_child(window);
    return window;
}

MenuSubwindow::ptr MenuSubwindow::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right)
{
    if (!parent) {
        throw std::runtime_error("MenuSubwindow::create(): Parent window does not exist");
    }
    auto window = MenuSubwindow::ptr(new MenuSubwindow(parent, Size(height, width), Margins(margin_top, margin_bottom, margin_left, margin_right)));
    parent->add_child(window);
    return window;
}

void MenuSubwindow::set_menu_columns(int columns)
{
    menu_columns = columns;
}

void MenuSubwindow::set_custom_menu_mark(const std::wstring& mark)
{
    menu_mark = mark;
    free(menu_mark_char);
    menu_mark_char = nu::wstring_to_char(mark);
    menu_mark_length = mark.length();
}

void MenuSubwindow::set_active_items_attrs(chtype attrs)
{
    active_item_attrs = attrs;
}

void MenuSubwindow::set_inactive_items_attrs(chtype attrs)
{
    inactive_item_attrs = attrs;
}

void MenuSubwindow::set_unfocused_items_attrs(chtype attrs)
{
    unfocused_item_attrs = attrs;
}

void MenuSubwindow::set_defined_item_attrs(chtype attrs)
{
    defined_item_attrs = attrs;
}

int MenuSubwindow::get_defined_item_index()
{
    return defined_item_index;
}

void MenuSubwindow::set_defined_item(int index)
{
    if (index < 0 || index >= (int)menu_items.size() - 1) {
        throw std::out_of_range("MenuWindow::set_defined_item(): index out of range");
    }
    defined_item_index = index;
}

void MenuSubwindow::reset_defined_item()
{
    defined_item_index = -1;
}

int MenuSubwindow::get_current_item_index()
{
    return item_index(current_item(menu.get()));
}

void MenuSubwindow::add_menu_item(const std::string& item, const std::string& desc, 
                                  Callback callback, std::vector<wint_t> keys)
{
    add_menu_item(nu::string_to_wstring(item), nu::string_to_wstring(desc), callback, keys);
}

void MenuSubwindow::add_menu_item(const std::wstring& item, const std::wstring& desc, 
                                  Callback callback, std::vector<wint_t> keys)
{
    char* name_char = nu::wstring_to_char(item);
    char* desc_char = nu::wstring_to_char(desc);
    menu_items_names.push_back(name_char);
    menu_items_descs.push_back(desc_char);

    ITEM* newItem = new_item(name_char, desc_char);
    if (!newItem) {
        free(name_char);
        free(desc_char);
        throw std::runtime_error("MenuWindow::add_menu_item(): Failed to create menu item");
    }

    if (menu_items.empty()) {
        menu_items.push_back((ITEM*)nullptr);
    }
    menu_items.insert(--menu_items.end(), newItem);
    if (callback) {
        auto pair = std::make_pair(keys, callback);
        menu_callbacks.push_back(pair);
    }
    else {
        auto pair = std::make_pair(std::vector<wint_t>{}, nullptr);
        menu_callbacks.push_back(pair);
    }

    max_item_length = get_max_item_length();
}

void MenuSubwindow::add_exit_callback(Callback callback, std::vector<wint_t> keys)
{
    if (callback) {
        auto pair = std::make_pair(keys, callback);
        exit_callbacks.push_back(pair);
    }
}

void MenuSubwindow::move_to(unsigned new_y, unsigned new_x)
{
    if (menu) {
        unpost_menu(menu.get());
    }
    BaseWindow::move_to(new_y, new_x);
    if (menu) {
        update_menu();
    }
}

void MenuSubwindow::resize(unsigned height, unsigned width)
{
    if (menu) {
        unpost_menu(menu.get());
    }
    BaseWindow::resize(height, width);
    if (menu) {
        update_menu();
    }
}

void MenuSubwindow::update(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    if (menu) {
        unpost_menu(menu.get());
    }
    BaseWindow::update(height, width, start_y, start_x);
    if (menu) {
        update_menu();
    }
}

void MenuSubwindow::show()
{
    if (is_showing() && !nu::display::needs_redraw(this)) {
        return;
    }
    
    state |= State::Showing;

    update_menu();

    nu::display::windows_manager.top_window_and_update(this);
    on_show();
}

void MenuSubwindow::hide()
{
    if (menu) {
        unpost_menu(menu.get());
    }
    stop();
    BaseWindow::hide();
}

void MenuSubwindow::set_focus(bool on)
{
    BaseWindow::set_focus(on);
    set_styles();
    draw_mark(item_index(current_item(menu.get())), true);
    update_panels();
}

void MenuSubwindow::run()
{
    if (!win) {
        throw std::runtime_error("MenuWindow::run(): Menu window is not created");
    }
    if (nu::display::set_running_window(this) == ERR) {
        return;
    }

    if (!is_focused()) {
        set_focus(true);
        draw_menu(true);
        doupdate();
    }

    is_running_ = true;
    while (is_running_) {
        if (!is_showing()) {
            set_focus(false);
            nu::display::show_all_windows();
            return;
        }

        wint_t wch = nu::wget_wch(win.get(), nu::work_mode);
        if (execute_exit_callback(wch) || wch == K_ESCAPE) {
            set_focus(false);
            is_running_ = false;
            nu::display::show_all_windows();
            return;
        }
        custom_menu_driver(wch);
        doupdate();
    }

    if (is_running_) {
        stop();
    }
}

void MenuSubwindow::stop()
{
    if (is_running_) {
        nu::display::reset_running_window();
    }
    is_running_ = false;
    if (is_showing()) {
        set_focus(false);
        draw_menu(false);
        doupdate();
    }
}

bool MenuSubwindow::is_running()
{
    return is_running_;
}

int MenuSubwindow::get_max_item_length()
{
    size_t max_length = 0;
    for (auto& item : menu_items_names) {
        std::wstring item_wstring = nu::string_to_wstring(item);
        // обрезаем пробелы в конце
        item_wstring = nu::rtrim(item_wstring);
        if (item_wstring.length() > max_length) {
            max_length = item_wstring.length();
        }
    }
    return max_length;
}

void MenuSubwindow::set_item_style(int index, chtype attrs)
{
    if (index < 0 || index >= (int)menu_items.size() - 1) {
        throw std::out_of_range("MenuWindow::set_item_style(): index out of range");
    }
    attr_t attrs_only = attrs & ~A_COLOR;
    short color_pair = PAIR_NUMBER(attrs);

    attr_t active_item_attrs_only = active_item_attrs & ~A_COLOR;
    short active_color_pair = PAIR_NUMBER(active_item_attrs);

    // attr_t unfocused_item_attrs_only = unfocused_item_attrs & ~A_COLOR;
    // short unfocused_color_pair = PAIR_NUMBER(unfocused_item_attrs);

    if (menu_items.size() - 1 > (size_t)size.height) {
        int top_row_index = top_row(menu.get());
        int bottom_row_index = top_row_index + size.height - 1;
        if (index < top_row_index || index > bottom_row_index) {
            return;
        }
    }
    
    if (is_focused()) {
        if (current_item_index == defined_item_index) {
            mvwchgat(win.get(), index, menu_mark_length, max_item_length, active_item_attrs_only, active_color_pair, nullptr);
        }
        else {
            mvwchgat(win.get(), index, menu_mark_length, max_item_length, attrs_only, color_pair, nullptr);
        }
    }
    else {
        // mvwchgat(menu_window, index, menu_mark_length, max_item_length, unfocused_item_attrs_only, unfocused_color_pair, nullptr);
    }
}

void MenuSubwindow::set_styles()
{
    if (is_focused()) {
        set_menu_fore(menu.get(), active_item_attrs);
        // set_item_style(current_item_index, active_item_attrs);
    }
    else {
        set_menu_fore(menu.get(), unfocused_item_attrs);
        // set_item_style(current_item_index, unfocused_item_attrs);
    }

    set_menu_back(menu.get(), inactive_item_attrs);

    // применяем стиль к defined
    set_item_style(defined_item_index, defined_item_attrs);
}

void MenuSubwindow::draw_mark(int row, bool on)
{
    if (row < 0 || row >= (int)menu_items.size() - 1) {
        return;
    }

    int y = 0;
    int x = 0;
    int top_row_index = top_row(menu.get());

    if (menu_columns == 1) {
        y = row - top_row_index;
        x = 0;
    }
    else {
        int total = menu_items.size() - 1;
        int cols = menu_columns;
        int rows = (total + cols - 1) / cols;

        int col_width = 0;
        for (int i = 0; i < total; i++) {
            int item_length = nu::string_to_wstring(menu_items_names[i]).length();
            col_width = std::max(col_width, item_length);
        }

        // определяем колонку и строку внутри колонки
        int col = row / rows;
        int row_in_col = row % rows;

        y = row_in_col - top_row_index;
        x = col * (col_width + menu_mark_length + 1);   // menu_mark_length - длина маркера
                                                        // 1 - стандартный отступ между столбиками
    }

    if (on) {
        mvwaddwstr(win.get(), y, x, menu_mark.c_str());   // показать
    }
    else {
        auto mark_length = std::wstring(menu_mark.length(), L' ');
        mvwaddwstr(win.get(), y, x, mark_length.c_str());     // стереть
    }
}

void MenuSubwindow::draw_scrollbar()
{
    int total_items = menu_items.size() - 1;
    int cols = menu_columns;
    int rows_to_draw = size.height;
    int total_rows = (total_items + cols - 1) / cols;
    
    if (total_rows <= rows_to_draw) {
        return;
    }

    int max_scroll = total_rows - rows_to_draw;
    float ratio = (float)rows_to_draw / (float)total_rows;
    int thumb_h = std::max(1, (int)(rows_to_draw * ratio));    // длина ползунка
    int track = rows_to_draw - thumb_h;

    int top_row_index = top_row(menu.get());

    int thumb_y;    // позиция ползунка
    // если самый верхний элемент, ползунок должен быть в самом верху
    if (top_row_index == 0) {
        thumb_y = 0;
    }
    // если достигли конечного пункта меню, ползунок должен быть в самом низу
    else if (top_row_index >= max_scroll) {
        thumb_y = track;
    }
    // середину рассчитываем по формуле
    else {
        thumb_y = 1 + ((top_row_index - 1) * (track - 1)) / (max_scroll - 1);
    }
    int thumb_x = size.width - 1;

    // полоска
    for (unsigned i = 0; i < size.height; i++) {
        mvwaddch(win.get(), i, thumb_x, ACS_VLINE);
    }
    // ползунок
    for (int i = 0; i < thumb_h; i++) {
        mvwaddwstr(win.get(), thumb_y + i, thumb_x, L"█");
    }
}

bool MenuSubwindow::execute_exit_callback(wint_t ch)
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

bool MenuSubwindow::execute_menu_callback(int index, wint_t ch) {
    const auto& pair = menu_callbacks[index];
    const auto& keys = pair.first;
    const auto& callback = pair.second;

    bool found = false;
    for (auto& key : keys) {
        if (key == ch) {
            if (callback) {
                callback();
                found = true;
            }
            break;
        }
    }

    return found;
}

bool MenuSubwindow::custom_menu_driver(int ch)
{
    int old_index = item_index(current_item(menu.get()));

    // обрабатываем коллбэки
    if (execute_menu_callback(old_index, ch)) {
        return true;
    }

    // стираем
    draw_mark(old_index, false);

    // передаем сигнал оригинальному драйверу
    int rc = E_OK;
    switch (ch) {
        case KEY_UP:
            rc = menu_driver(menu.get(), REQ_PREV_ITEM);
            break;
        case KEY_DOWN:
            rc = menu_driver(menu.get(), REQ_NEXT_ITEM);
            break;
        case KEY_LEFT:
            if (menu_columns == 1) {
                break;
            }
            rc = menu_driver(menu.get(), REQ_LEFT_ITEM);
            break;
        case KEY_RIGHT:
            if (menu_columns == 1) {
                break;
            }
            rc = menu_driver(menu.get(), REQ_RIGHT_ITEM);
            break;
        case KEY_PPAGE:
            rc = menu_driver(menu.get(), REQ_SCR_UPAGE);
            break;
        case KEY_NPAGE:
            rc = menu_driver(menu.get(), REQ_SCR_DPAGE);
            break;
        default:
            rc = menu_driver(menu.get(), ch);
            break;
    }

    // если сигнал не опознан,
    // рисуем стертую метку и возвращаем управление в main-loop
    if (rc == E_REQUEST_DENIED) {
        draw_mark(old_index, true);
        return false;
    }

    current_item_index = item_index(current_item(menu.get()));
    draw_menu(is_focused());
    return true;
}

void MenuSubwindow::draw_menu(bool is_focused)
{
    if (is_focused) {
        state |= State::Focused;
    }
    else {
        state &= ~State::Focused;
    }
    set_styles();
    draw_mark(current_item_index, true);
    draw_scrollbar();
    update_panels();
}

void MenuSubwindow::update_menu()
{
    if (menu_items.empty()) {
        return;
    }

    if (menu) {
        current_item_index = item_index(current_item(menu.get()));

        menu.reset();
    }
    if (panel) {
        panel.reset();
    }
    if (win) {
        win.reset();
    }

    win = WINDOW_ptr(derwin(
        get_parent()->get_win(), 
        size.height, size.width, 
        position.start_y, position.start_x
    ));
    if (!win) {
        throw std::runtime_error("MenuWindow::update_menu(): Failed to create window");
    }
    panel = PANEL_ptr(new_panel(win.get()));
    if (!panel) {
        throw std::runtime_error("MenuWindow::update_menu(): Failed to create panel");
    }

    menu = MENU_ptr(new_menu(menu_items.data()));
    if (!menu) {
        throw std::runtime_error("MenuWindow::update_menu(): Failed to create menu");
    }

    set_menu_win(menu.get(), get_parent()->get_win());
    // set_menu_win(menu, win);
    set_menu_sub(menu.get(), win.get());

    keypad(win.get(), true);
    menu_opts_off(menu.get(), O_ROWMAJOR);
    set_menu_format(menu.get(), size.height, menu_columns);

    show_panel(panel.get());

    update_panels();

    set_current_item(menu.get(), menu_items[current_item_index]);

    set_styles();
    // создаем строку из пробелов длиной с маркер
    std::string temp = std::string(menu_mark.length(), ' ');
    set_menu_mark(menu.get(), temp.c_str());

    post_menu(menu.get());
    draw_mark(item_index(current_item(menu.get())), true);
    draw_scrollbar();

    update_panels();
    doupdate();
}