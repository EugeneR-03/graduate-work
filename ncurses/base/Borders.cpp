#include "Borders.hpp"

template<typename T>
void DSBorder<T>::init_borders_map()
{
    this->borders_map["horizontal"] = this->ts;
    this->borders_map["vertical"] = this->ls;
    this->borders_map["top_left_corner"] = this->tl;
    this->borders_map["top_right_corner"] = this->tr;
    this->borders_map["bottom_left_corner"] = this->bl;
    this->borders_map["bottom_right_corner"] = this->br;
    this->borders_map["horizontal_to_left"] = this->horizontal_to_left;
    this->borders_map["horizontal_to_right"] = this->horizontal_to_right;
    this->borders_map["vertical_to_top"] = this->vertical_to_top;
    this->borders_map["vertical_to_bottom"] = this->vertical_to_bottom;
}

Border::Border(char ls, char rs, char ts, char bs, char tl, char tr, char bl, char br,
                   char horizontal_to_left, char horizontal_to_right, char vertical_to_top, char vertical_to_bottom) 
    : DSBorder(ls, rs, ts, bs, tl, tr, bl, br, horizontal_to_left, horizontal_to_right, vertical_to_top, vertical_to_bottom)
{
    init_borders_map();
}

Border::Border(Type type)
{
    switch (type) {
        case Type::None:
            ls = ' ';
            rs = ' ';
            ts = ' ';
            bs = ' ';
            tl = ' ';
            tr = ' ';
            bl = ' ';
            br = ' ';
            horizontal_to_left = ' ';
            horizontal_to_right = ' ';
            vertical_to_top = ' ';
            vertical_to_bottom = ' ';
            break;
            
        case Type::Default:
            ls = ACS_VLINE;
            rs = ACS_VLINE;
            ts = ACS_HLINE;
            bs = ACS_HLINE;
            tl = ACS_ULCORNER;
            tr = ACS_URCORNER;
            bl = ACS_LLCORNER;
            br = ACS_LRCORNER;
            horizontal_to_left = ACS_LTEE;
            horizontal_to_right = ACS_RTEE;
            vertical_to_top = ACS_TTEE;
            vertical_to_bottom = ACS_BTEE;
            break;

        case Type::Thick:
            ls = ACS_BLOCK;
            rs = ACS_BLOCK;
            ts = ACS_BLOCK;
            bs = ACS_BLOCK;
            tl = ACS_BLOCK;
            tr = ACS_BLOCK;
            bl = ACS_BLOCK;
            br = ACS_BLOCK;
            horizontal_to_left = ACS_BLOCK;
            horizontal_to_right = ACS_BLOCK;
            vertical_to_top = ACS_BLOCK;
            vertical_to_bottom = ACS_BLOCK;
            break;
    }

    init_borders_map();
}

void Border::wdraw(WINDOW* win) const {
    if (!win) {
        throw std::runtime_error("border::wdraw: win == nullptr");
    }

    wborder(win, ls, rs, ts, bs, tl, tr, bl, br);
}

void Border::wclear(WINDOW* win) const {
    if (!win) {
        throw std::runtime_error("border::wclear: win == nullptr");
    }

    wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
}

const wchar_t Border::get_border(std::string border_place) const
{
    if (!borders_map.contains(border_place)) {
        throw std::runtime_error("border::get_border: border_place not found");
    }
    return borders_map.at(border_place);
}

BorderW::BorderW(wchar_t ls, wchar_t rs, wchar_t ts, wchar_t bs, wchar_t tl, wchar_t tr, wchar_t bl, wchar_t br,
                     wchar_t horizontal_to_left, wchar_t horizontal_to_right, wchar_t vertical_to_top, wchar_t vertical_to_bottom) 
    : DSBorder(ls, rs, ts, bs, tl, tr, bl, br, horizontal_to_left, horizontal_to_right, vertical_to_top, vertical_to_bottom)
{
    init_borders_map();
}

BorderW::BorderW(Type type)
{
    switch (type) {
        case Type::None:
            ls = BORDER_NONE;
            rs = BORDER_NONE;
            ts = BORDER_NONE;
            bs = BORDER_NONE;
            tl = BORDER_NONE;
            tr = BORDER_NONE;
            bl = BORDER_NONE;
            br = BORDER_NONE;
            horizontal_to_left =    BORDER_NONE;
            horizontal_to_right =   BORDER_NONE;
            vertical_to_top =       BORDER_NONE;
            vertical_to_bottom =    BORDER_NONE;
            break;

        case Type::Default:
            ls = BORDER_DEFAULT_VERTICAL;
            rs = BORDER_DEFAULT_VERTICAL;
            ts = BORDER_DEFAULT_HORIZONTAL;
            bs = BORDER_DEFAULT_HORIZONTAL;
            tl = BORDER_DEFAULT_TOP_LEFT;
            tr = BORDER_DEFAULT_TOP_RIGHT;
            bl = BORDER_DEFAULT_BOTTOM_LEFT;
            br = BORDER_DEFAULT_BOTTOM_RIGHT;
            horizontal_to_left =    BORDER_DEFAULT_HORIZONTAL_TO_LEFT;
            horizontal_to_right =   BORDER_DEFAULT_HORIZONTAL_TO_RIGHT;
            vertical_to_top =       BORDER_DEFAULT_VERTICAL_TO_TOP;
            vertical_to_bottom =    BORDER_DEFAULT_VERTICAL_TO_BOTTOM;
            break;

        case Type::Thick:
            ls = BORDER_THICK_VERTICAL;
            rs = BORDER_THICK_VERTICAL;
            ts = BORDER_THICK_HORIZONTAL;
            bs = BORDER_THICK_HORIZONTAL;
            tl = BORDER_THICK_TOP_LEFT;
            tr = BORDER_THICK_TOP_RIGHT;
            bl = BORDER_THICK_BOTTOM_LEFT;
            br = BORDER_THICK_BOTTOM_RIGHT;
            horizontal_to_left  = BORDER_THICK_HORIZONTAL_TO_LEFT;
            horizontal_to_right = BORDER_THICK_HORIZONTAL_TO_RIGHT;
            vertical_to_top     = BORDER_THICK_VERTICAL_TO_TOP;
            vertical_to_bottom  = BORDER_THICK_VERTICAL_TO_BOTTOM;
            break;

        case Type::Double:
            ls = BORDER_DOUBLE_VERTICAL;
            rs = BORDER_DOUBLE_VERTICAL;
            ts = BORDER_DOUBLE_HORIZONTAL;
            bs = BORDER_DOUBLE_HORIZONTAL;
            tl = BORDER_DOUBLE_TOP_LEFT;
            tr = BORDER_DOUBLE_TOP_RIGHT;
            bl = BORDER_DOUBLE_BOTTOM_LEFT;
            br = BORDER_DOUBLE_BOTTOM_RIGHT;
            horizontal_to_left  = BORDER_DOUBLE_HORIZONTAL_TO_LEFT;
            horizontal_to_right = BORDER_DOUBLE_HORIZONTAL_TO_RIGHT;
            vertical_to_top     = BORDER_DOUBLE_VERTICAL_TO_TOP;
            vertical_to_bottom  = BORDER_DOUBLE_VERTICAL_TO_BOTTOM;
            break;
    }

    init_borders_map();
}

void BorderW::wdraw(WINDOW* win) const {
    if (!win) {
        throw std::runtime_error("border::wdraw: win == nullptr");
    }

    int max_y = getmaxy(win);
    int max_x = getmaxx(win);

    // если слишком маленькое окно
    if (max_y < 2 || max_x < 2) {
        return;
    }

    werase(win);

    // рисуем углы (по одному символу длиной 1)
    // формируем буферы для широких символов
    wchar_t buf[2] = { L'\0', L'\0' };

    buf[0] = tl;
    mvwaddnwstr(win, 0, 0, buf, 1);
    buf[0] = tr;
    mvwaddnwstr(win, 0, max_x-1, buf, 1);
    buf[0] = bl;
    mvwaddnwstr(win, max_y-1, 0, buf, 1);
    buf[0] = br;
    mvwaddnwstr(win, max_y-1, max_x-1, buf, 1);

    // горизонтальные линии
    buf[0] = ts;
    for (int x = 1; x < max_x - 1; ++x) {
        mvwaddnwstr(win, 0, x, buf, 1);
    }
    buf[0] = bs;
    for (int x = 1; x < max_x - 1; ++x) {
        mvwaddnwstr(win, max_y-1, x, buf, 1);
    }

    // вертикальные линии
    buf[0] = ls;
    for (int y = 1; y < max_y - 1; ++y) {
        mvwaddnwstr(win, y, 0, buf, 1);
    }
    buf[0] = rs;
    for (int y = 1; y < max_y - 1; ++y) {
        mvwaddnwstr(win, y, max_x-1, buf, 1);
    }
}

void BorderW::wclear(WINDOW* win) const {
    if (!win) {
        throw std::runtime_error("border::wclear: win == nullptr");
    }

    wborder(win, L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ');
}

const wchar_t BorderW::get_border(std::string border_place) const {
    if (!borders_map.contains(border_place)) {
        throw std::runtime_error("border::get_border: border_place not found");
    }
    return borders_map.at(border_place);
}