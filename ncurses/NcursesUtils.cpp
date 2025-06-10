#include "NcursesUtils.hpp"

nu::WindowsManager::WindowsManager() {}
nu::WindowsManager::~WindowsManager()
{
    this->clear();
}

void nu::WindowsManager::add_window(const BaseWindow* window)
{
    active_windows.push_back(WindowPointer(window, true));
    update_window(window);
}

void nu::WindowsManager::remove_window(const BaseWindow* window)
{
    for (auto it = active_windows.begin(); it != active_windows.end(); it++) {
        if (it->window == window) {
            active_windows.erase(it);
            break;
        }
    }
}

void nu::WindowsManager::remove_window_and_update(const BaseWindow* window)
{
    remove_window(window);
    update();
}

void nu::WindowsManager::update_window(const BaseWindow* window)
{
    int window_height, window_width;
    int window_start_y, window_start_x;
    getmaxyx(window->get_win(), window_height, window_width);
    getbegyx(window->get_win(), window_start_y, window_start_x);
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            int absolute_y = window_start_y + y;
            int absolute_x = window_start_x + x;
            windows_on_screen[absolute_y][absolute_x] = const_cast<BaseWindow*>(window);
        }
    }
}

void nu::WindowsManager::top_window(const BaseWindow* window)
{
    for (auto it = active_windows.begin(); it != active_windows.end(); it++) {
        if (it->window == window) {
            active_windows.erase(it);
            break;
        }
    }
    active_windows.push_back(WindowPointer(window, true));
}

void nu::WindowsManager::top_window_and_update(const BaseWindow* window)
{
    top_window(window);
    update();
}

void nu::WindowsManager::hide_window(const BaseWindow* window)
{
    for (auto it = active_windows.begin(); it != active_windows.end(); it++) {
        if (it->window == window) {
            active_windows.erase(it);
            break;
        }
    }
}

void nu::WindowsManager::hide_window_and_update(const BaseWindow* window)
{
    hide_window(window);
    update();
}

const BaseWindow* nu::WindowsManager::check_presence(unsigned y, unsigned x)
{
    if (!windows_on_screen.contains(y)) {
        return nullptr;
    }
    if (!windows_on_screen[y].contains(x)) {
        return nullptr;
    }
    return windows_on_screen[y][x];
}

void nu::WindowsManager::mark_window_for_redrawing(const BaseWindow* window, bool needs_updating)
{
    for (auto& window_pointer : active_windows) {
        if (window_pointer.window == window) {
            window_pointer.needs_redrawing = needs_updating;
        }
    }
}

bool nu::WindowsManager::needs_redraw(const BaseWindow* window)
{
    for (auto& window_pointer : active_windows) {
        if (window_pointer.window == window) {
            return window_pointer.needs_redrawing;
        }
    }
    return false;
}

void nu::WindowsManager::update()
{
    windows_on_screen.clear();
    for (auto& window_pointer : active_windows) {
        update_window(window_pointer.window);
    }
}

void nu::WindowsManager::mark_all(Mark mark)
{
    if (has_flag(mark, Mark::Redraw)) {
        for (auto& window_pointer : active_windows) {
            window_pointer.needs_redrawing = true;
        }
    }
    if (has_flag(mark, Mark::Update)) {
        update();
    }
}

void nu::WindowsManager::show_all()
{
    for (auto& window_pointer : active_windows) {
        if (window_pointer.needs_redrawing) {
            window_pointer.window->show();
            window_pointer.needs_redrawing = false;
        }
    }
}

void nu::WindowsManager::clear()
{
    active_windows.clear();
    windows_on_screen.clear();
}

IRunnable* nu::WindowsManager::get_running_window()
{
    return running_window;
}

IRunnable* nu::WindowsManager::get_last_running_window()
{
    return last_running_window;
}

int nu::WindowsManager::set_running_window(IRunnable* window)
{
    if (running_window == window) {
        return ERR;
    }

    if (running_window) {
        running_window->stop();
    }
    last_running_window = running_window;
    running_window = window;
    return OK;
}

void nu::WindowsManager::reset_running_window()
{
    if (running_window) {
        running_window = nullptr;
    }
}

void nu::display::update_size_and_center()
{
    getmaxyx(stdscr, display::size.height, display::size.width);
    display::center = Position(display::size.height / 2, display::size.width / 2);
}

void nu::display::set_redraw_function(Callback callback)
{
    redraw_function = callback;
}

bool nu::display::needs_redraw(const BaseWindow* window)
{
    return windows_manager.needs_redraw(window);
}

void nu::display::mark_window(const BaseWindow& window, Mark mark)
{
    nu::display::mark_window(&window, mark);
}

void nu::display::mark_window(const BaseWindow* window, Mark mark)
{
    if (has_flag(mark, Mark::Update)) {
        windows_manager.update_window(window);
    }
    if (has_flag(mark, Mark::Redraw)) {
        windows_manager.mark_window_for_redrawing(window, true);
    }
}

void nu::display::mark_all_windows(Mark mark)
{
    windows_manager.mark_all(mark);
}

void nu::display::show_all_windows()
{
    windows_manager.show_all();
}

IRunnable* nu::display::get_running_window()
{
    return windows_manager.get_running_window();
}

IRunnable* nu::display::get_last_running_window()
{
    return windows_manager.get_last_running_window();
}

int nu::display::set_running_window(IRunnable* window)
{
    return windows_manager.set_running_window(window);
}

void nu::display::reset_running_window()
{
    windows_manager.reset_running_window();
}

void nu::mouse::update_mouse_position(unsigned y, unsigned x)
{
    if (y < 0 || x < 0) {
        throw std::out_of_range("nu::mouse::update_mouse_position: y or x < 0");
    }
    if (y >= display::size.height || x >= display::size.width) {
        throw std::out_of_range("nu::mouse::update_mouse_position: y or x >= size");
    }
    if (mouse::position.start_y == y && mouse::position.start_x == x) {
        return;
    }
    mouse::previous_position = mouse::position;
    mouse::position = Position(y, x);
}

void nu::enable_window_events()
{
#ifdef _WIN32
    DWORD mode;
    GetConsoleMode(display::h_in, &mode);
    // mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    mode |= ENABLE_WINDOW_INPUT;
    mode &= ~ENABLE_VIRTUAL_TERMINAL_INPUT;
    mode &= ~ENABLE_QUICK_EDIT_MODE;    // для обработки событий мыши
    SetConsoleMode(display::h_in, mode);
    // CloseHandle(display::h_in);
#endif
}

void nu::set_quick_edit_mode(bool on)
{
#ifdef _WIN32
    DWORD mode;
    GetConsoleMode(display::h_in, &mode);
    if (on) {
        mode |= ENABLE_QUICK_EDIT_MODE;
    } else {
        mode &= ~ENABLE_QUICK_EDIT_MODE;
    }
    SetConsoleMode(display::h_in, mode);
#endif
}

void nu::toggle_quick_edit_mode()
{
#ifdef _WIN32
    DWORD mode;
    GetConsoleMode(display::h_in, &mode);
    mode ^= ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(display::h_in, mode);
#endif
}

void nu::init(WorkMode mode)
{
#ifdef _WIN32
    display::h_in = GetStdHandle(STD_INPUT_HANDLE);
    display::h_out = GetStdHandle(STD_OUTPUT_HANDLE);

    enable_window_events();
#endif

    initscr();
    cbreak();
    noecho();
    start_color();
    curs_set(0);
    keypad(stdscr, true);
    work_mode |= mode;
    if (has_flag(work_mode, WorkMode::WithMouse)) {
        mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);
    }

    init_pair(Black, COLOR_BLACK, COLOR_BLACK);
    init_pair(Red, COLOR_RED, COLOR_BLACK);
    init_pair(Green, COLOR_GREEN, COLOR_BLACK);
    init_pair(Yellow, COLOR_YELLOW, COLOR_BLACK);
    init_pair(Blue, COLOR_BLUE, COLOR_BLACK);
    init_pair(Magenta, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(Cyan, COLOR_CYAN, COLOR_BLACK);
    init_pair(White, COLOR_WHITE, COLOR_BLACK);

    resize_term(0, 0);
    clearok(stdscr, true);
    clear();
    refresh();

    getmaxyx(stdscr, display::size.height, display::size.width);
    display::center = Position(display::size.height / 2, display::size.width / 2);

    nu::display::set_redraw_function([]() {
        nu::display::mark_all_windows(Mark::Update | Mark::Redraw);
        nu::display::show_all_windows();
    });
}

void nu::end()
{
#ifdef _WIN32
    CloseHandle(display::h_in);
    CloseHandle(display::h_out);
#endif
    display::windows_manager.clear();
    endwin();
}

char* nu::wstring_to_char(const std::wstring& wstring)
{
    std::string utf8 = boost::nowide::narrow(wstring);
    char* buf = static_cast<char*>(std::malloc(utf8.size() + 1));
    if (!buf) throw std::bad_alloc();
    std::memcpy(buf, utf8.data(), utf8.size() + 1);
    return buf;
}

std::wstring nu::char_to_wstring(const char* string)
{
    return boost::nowide::widen(string);
}

std::string nu::wstring_to_string(const std::wstring& wstring)
{
    return boost::nowide::narrow(wstring);
}

std::wstring nu::string_to_wstring(const std::string& string)
{
    return boost::nowide::widen(string);
}

std::string nu::ltrim(const std::string& str)
{
    std::string result = str;
    while (!result.empty() && std::isspace(result.front())) {
        result.erase(result.begin());
    }
    return result;
}

std::wstring nu::ltrim(const std::wstring& str)
{
    std::wstring result = str;
    while (!result.empty() && std::iswspace(result.front())) {
        result.erase(result.begin());
    }
    return result;
}

std::string nu::rtrim(const std::string& str)
{
    std::string result = str;
    while (!result.empty() && std::isspace(result.back())) {
        result.pop_back();
    }
    return result;
}

std::wstring nu::rtrim(const std::wstring& str)
{
    std::wstring result = str;
    while (!result.empty() && std::iswspace(result.back())) {
        result.pop_back();
    }
    return result;
}

wchar_t nu::wget_wch(WINDOW* win, WorkMode mode)
{
#ifdef _WIN32
    return get_wch_by_read_console(mode);
#else
    return wget_wch(win);
#endif
}

wchar_t nu::get_wch_by_read_console(WorkMode mode)
{
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD ir;
    DWORD numRead;

    while (true) {
        if (!ReadConsoleInputW(hIn, &ir, 1, &numRead)) {
            return 0;
        }

        switch (ir.EventType) {
            case WINDOW_BUFFER_SIZE_EVENT: {
                int new_h = ir.Event.WindowBufferSizeEvent.dwSize.Y;
                int new_w = ir.Event.WindowBufferSizeEvent.dwSize.X;

                // обновляем curses
                resizeterm(new_h, new_w);
                refresh();
                nu::display::update_size_and_center();

                // пересоздаём/перерисовываем
                if (nu::display::redraw_function) {
                    nu::display::redraw_function();
                }

                break;
            }

            case KEY_EVENT: {
                const KEY_EVENT_RECORD &ker = ir.Event.KeyEvent;

                if (!ker.bKeyDown) {
                    continue;
                }

                switch (ker.wVirtualKeyCode) {
                    case VK_LEFT:
                        return KEY_LEFT;
                    case VK_RIGHT:
                        return KEY_RIGHT;
                    case VK_UP:
                        return KEY_UP;
                    case VK_DOWN:
                        return KEY_DOWN;
                    case VK_RETURN:
                        return KEY_ENTER;
                    case VK_BACK:
                        return KEY_BACKSPACE;
                    case VK_ESCAPE:
                        return K_C_ESCAPE;
                    case VK_DELETE:
                        return KEY_DC;
                    case VK_HOME:
                        return KEY_HOME;
                    case VK_END:
                        return KEY_END;
                    case VK_TAB:
                        return 9;
                    default:
                        if (ker.uChar.UnicodeChar != 0) {
                            return ker.uChar.UnicodeChar;
                        }
                        
                        BYTE kbd[256];
                        GetKeyboardState(kbd);
                        wchar_t buf[4];
                        int n = ToUnicodeEx(ker.wVirtualKeyCode, ker.wVirtualScanCode,
                                            kbd, buf, 4, 0, GetKeyboardLayout(0));

                        if (n == 1)     // нормальный символ
                            return buf[0];
                        if (n < 0) {    // очищаем dead-key
                            ToUnicodeEx(ker.wVirtualKeyCode, ker.wVirtualScanCode,
                                        kbd, buf, 4, 0, GetKeyboardLayout(0));
                            continue;
                        }
                        break;
                }

                break;
            }

            case MOUSE_EVENT: {
                if (!has_flag(mode, WorkMode::WithMouse)) {
                    continue;
                }

                const MOUSE_EVENT_RECORD& mer = ir.Event.MouseEvent;
                int global_y = mer.dwMousePosition.Y;
                int global_x = mer.dwMousePosition.X;

                if (Flags::has_flag(mer.dwEventFlags, MOUSE_MOVED)) {
                    nu::mouse::update_mouse_position(global_y, global_x);
                    auto old_position_window = nu::display::windows_manager.check_presence(nu::mouse::previous_position.start_y, nu::mouse::previous_position.start_x);
                    auto window = nu::display::windows_manager.check_presence(nu::mouse::position.start_y, nu::mouse::position.start_x);
                    
                    if (window != nullptr) {
                        window->on_hover(nu::mouse::position.start_y, nu::mouse::position.start_x);
                    }
                    if (old_position_window != nullptr && old_position_window != window) {
                        old_position_window->on_unhover();
                    }
                }

                if (Flags::has_flag(mer.dwButtonState, FROM_LEFT_1ST_BUTTON_PRESSED)) {
                    auto window = nu::display::windows_manager.check_presence(nu::mouse::position.start_y, nu::mouse::position.start_x);
                    
                    if (window != nullptr) {
                        window->on_left_click(nu::mouse::position.start_y, nu::mouse::position.start_x);
                    }
                    return BUTTON1_CLICKED;
                }

                if (Flags::has_flag(mer.dwButtonState, RIGHTMOST_BUTTON_PRESSED)) {
                    auto window = nu::display::windows_manager.check_presence(nu::mouse::position.start_y, nu::mouse::position.start_x);
                    
                    if (window != nullptr) {
                        window->on_right_click(nu::mouse::position.start_y, nu::mouse::position.start_x);
                    }
                    return BUTTON2_CLICKED;
                }

                break;
            }
        }
    }
}

void nu::wprint_text(WINDOW* win, TextW text, Size size, Position position)
{
    if (!win) {
        throw std::runtime_error("nu::wprint_text: win == nullptr");
    }

    const int box_w = static_cast<int>(size.width);
    const int box_h = static_cast<int>(size.height);

    // разбивание строки на токены (слова или символы)
    std::vector<std::wstring> tokens;
    std::wstring buf;
    for (wchar_t ch : text.text) {
        if (ch == L'\n') {  // принудительный перевод строки
            if (!buf.empty()) {
                tokens.push_back(std::move(buf));
                buf.clear();
            }
            tokens.emplace_back(1, L'\n');  // отдельный токен
        }
        else if (iswspace(ch)) {
            if (!buf.empty()) {
                tokens.push_back(std::move(buf));
                buf.clear();
            }
        }
        else {
            buf += ch;
        }
    }
    if (!buf.empty()) {
        tokens.push_back(std::move(buf));
    }

    // сбор строк с учетом ширины
    std::vector<std::wstring> lines;
    std::wstring current;

    auto flush_current = [&] {
        if (!current.empty() || lines.empty()) {
            lines.push_back(std::move(current));
        }
        current.clear();
    };

    for (const auto& tok : tokens) {
        if (tok == L"\n") {     // явный перевод строки
            flush_current();
            continue;
        }

        const int word_len = static_cast<int>(tok.size());
        const int need = current.empty() ? word_len
                                         : current.size() + 1 + word_len;

        if (need <= box_w) {    // помещается в текущую строку
            if (!current.empty()) current += L' ';
            current += tok;
        }
        else {
            flush_current();
            // если слово шире строки - кладём как есть (будет обрезано при выводе)
            current = tok;
        }
    }
    flush_current();

    // вывод строк с нужным выравниваем
    wattron(win, text.attrs);

    const int max_lines = std::min<int>(box_h, lines.size());
    for (int i = 0; i < max_lines; ++i) {
        const std::wstring& line = lines[i];
        const int len = static_cast<int>(line.size());
        int y = position.start_y + i;
        int x = position.start_x;   // по умолчанию левое выравнивание

        switch (text.align) {
            case Align::Center:
                x += (box_w - len) / 2;
                break;

            case Align::Right:
                x += box_w - len;
                break;

            case Align::Justify:
                if (i != max_lines - 1) {   // по ширине (justify) (кроме последней строки - для нее левое выравнивание)
                    std::vector<std::wstring> parts;
                    std::wistringstream iss(line);
                    for (std::wstring w; iss >> w; ) parts.push_back(std::move(w));

                    if (parts.size() > 1) {
                        int words_len = 0;
                        for (auto& p : parts) words_len += p.size();
                        int gaps = parts.size() - 1;
                        int spaces = box_w - words_len;
                        int min_gap = spaces / gaps;
                        int extra = spaces % gaps;

                        std::wstring j;
                        for (size_t k = 0; k < parts.size(); ++k) {
                            j += parts[k];
                            if (k != parts.size() - 1) {
                                j.append(min_gap + (extra-- > 0 ? 1 : 0), L' ');
                            }
                        }
                        mvwaddnwstr(win, y, x, j.c_str(), box_w);
                        continue;
                    }
                }
                [[fallthrough]];

            case Align::Left:
            default:
                break;
        }

        mvwaddnwstr(win, y, x, line.c_str(), std::min(len, box_w));
    }

    wattroff(win, text.attrs);
}