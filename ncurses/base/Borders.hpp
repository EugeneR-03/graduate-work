#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <ncursesw/ncurses.h>

#define BORDER_NONE                 L' '

#define BORDER_DEFAULT_HORIZONTAL   L'─'
#define BORDER_DEFAULT_VERTICAL     L'│'
#define BORDER_DEFAULT_TOP_LEFT     L'┌'
#define BORDER_DEFAULT_TOP_RIGHT    L'┐'
#define BORDER_DEFAULT_BOTTOM_LEFT  L'└'
#define BORDER_DEFAULT_BOTTOM_RIGHT L'┘'
#define BORDER_DEFAULT_CROSS        L'┼'
#define BORDER_DEFAULT_HORIZONTAL_TO_LEFT   L'├'
#define BORDER_DEFAULT_HORIZONTAL_TO_RIGHT  L'┤'
#define BORDER_DEFAULT_VERTICAL_TO_TOP      L'┴'
#define BORDER_DEFAULT_VERTICAL_TO_BOTTOM   L'┬'

#define BORDER_THICK_HORIZONTAL   L'━'
#define BORDER_THICK_VERTICAL     L'┃'
#define BORDER_THICK_TOP_LEFT     L'┏'
#define BORDER_THICK_TOP_RIGHT    L'┓'
#define BORDER_THICK_BOTTOM_LEFT  L'┗'
#define BORDER_THICK_BOTTOM_RIGHT L'┛'
#define BORDER_THICK_CROSS        L'┼'
#define BORDER_THICK_HORIZONTAL_TO_LEFT   L'┣'
#define BORDER_THICK_HORIZONTAL_TO_RIGHT  L'┫'
#define BORDER_THICK_VERTICAL_TO_TOP      L'┻'
#define BORDER_THICK_VERTICAL_TO_BOTTOM   L'┳'

#define BORDER_DOUBLE_HORIZONTAL   L'═'
#define BORDER_DOUBLE_VERTICAL     L'║'
#define BORDER_DOUBLE_TOP_LEFT     L'╔'
#define BORDER_DOUBLE_TOP_RIGHT    L'╗'
#define BORDER_DOUBLE_BOTTOM_LEFT  L'╚'
#define BORDER_DOUBLE_BOTTOM_RIGHT L'╝'
#define BORDER_DOUBLE_CROSS        L'╬'
#define BORDER_DOUBLE_HORIZONTAL_TO_LEFT   L'╠'
#define BORDER_DOUBLE_HORIZONTAL_TO_RIGHT  L'╣'
#define BORDER_DOUBLE_VERTICAL_TO_TOP      L'╩'
#define BORDER_DOUBLE_VERTICAL_TO_BOTTOM   L'╦'

struct IBorder {
    virtual ~IBorder() {}

    virtual const wchar_t get_border(std::string border_place) const = 0;
    virtual void wdraw(WINDOW* win) const = 0;
    virtual void wclear(WINDOW* win) const = 0;
};

template <typename T>
struct DSBorder {
    T ls;
    T rs;
    T ts;
    T bs;
    T tl;
    T tr;
    T bl;
    T br;

    // спец. стыки
    T horizontal_to_left;
    T horizontal_to_right;
    T vertical_to_top;
    T vertical_to_bottom;

    std::unordered_map<std::string, T> borders_map;

    void init_borders_map();

    DSBorder() {}
    DSBorder(T ls, T rs, T ts, T bs, T tl, T tr, T bl, T br,
                T horizontal_to_left, T horizontal_to_right, T vertical_to_top, T vertical_to_bottom)
        : ls(ls), rs(rs), ts(ts), bs(bs), tl(tl), tr(tr), bl(bl), br(br),
            horizontal_to_left(horizontal_to_left), horizontal_to_right(horizontal_to_right), 
            vertical_to_top(vertical_to_top), vertical_to_bottom(vertical_to_bottom),
            borders_map() {}
};

struct Border : DSBorder<chtype>, IBorder {
    enum class Type { None, Default, Thick };

    Border(char ls, char rs, char ts, char bs, char tl, char tr, char bl, char br,
            char horizontal_to_left, char horizontal_to_right, char vertical_to_top, char vertical_to_bottom);
    explicit Border(Type type);
    bool operator==(const Border& other) const;

    const wchar_t get_border(std::string border_place) const override;
    void wdraw(WINDOW* win) const override;
    void wclear(WINDOW* win) const override;
};

struct BorderW : DSBorder<wchar_t>, IBorder {
    enum class Type { None, Default, Thick, Double };

    BorderW(wchar_t ls, wchar_t rs, wchar_t ts, wchar_t bs, wchar_t tl, wchar_t tr, wchar_t bl, wchar_t br,
            wchar_t horizontal_to_left, wchar_t horizontal_to_right, wchar_t vertical_to_top, wchar_t vertical_to_bottom);
    explicit BorderW(Type type);
    bool operator==(const BorderW& other) const;

    const wchar_t get_border(std::string border_place) const override;
    void wdraw(WINDOW* win) const override;
    void wclear(WINDOW* win) const override;
};