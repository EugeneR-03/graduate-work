#pragma once

#include <string>

namespace nu {
    std::string wstring_to_string(const std::wstring& string);
    std::wstring string_to_wstring(const std::string& string);
}

struct Size {
    unsigned height = 0;
    unsigned width = 0;

    Size() : height(0), width(0) {}
    Size(unsigned height, unsigned width) : height(height), width(width) {}
    Size(const Size& other) : height(other.height), width(other.width) {}

    bool operator==(const Size& other) const { return height == other.height && width == other.width; }
};

struct Position {
    unsigned start_y = 0;
    unsigned start_x = 0;

    Position() : start_y(0), start_x(0) {}
    Position(unsigned start_y, unsigned start_x) : start_y(start_y), start_x(start_x) {}
    Position(const Position& other) : start_y(other.start_y), start_x(other.start_x) {}

    bool operator==(const Position& other) const { return start_y == other.start_y && start_x == other.start_x; }
};

struct Margins {
    unsigned top = 0;
    unsigned bottom = 0;
    unsigned left = 0;
    unsigned right = 0;

    Margins() : top(0), bottom(0), left(0), right(0) {}
    explicit Margins(unsigned value) : top(value), bottom(value), left(value), right(value) {}
    Margins(unsigned top, unsigned bottom, unsigned left, unsigned right) : top(top), bottom(bottom), left(left), right(right) {}
    Margins(const Margins& other) : top(other.top), bottom(other.bottom), left(other.left), right(other.right) {}
    
    bool operator==(unsigned value) const { return top == value && bottom == value && left == value && right == value; }
    bool operator==(const Margins& other) const { return top == other.top && bottom == other.bottom && left == other.left && right == other.right; }
};

enum class Align {
    Left,
    Center,
    Right,
    Justify
};

struct TextW {
    std::wstring text;
    Align align;
    chtype attrs;

    TextW()
        : align(Align::Left), attrs(A_NORMAL) {}
    explicit TextW(const std::wstring& wtext, Align align = Align::Left, chtype attrs = A_NORMAL)
        : text(wtext), align(align), attrs(attrs) {}
    explicit TextW(const std::string& text, Align align = Align::Left, chtype attrs = A_NORMAL)
    {
        this->text = nu::string_to_wstring(text);
        this->align = align;
        this->attrs = attrs;
    }
    TextW(const TextW& other)
        : text(other.text), align(other.align), attrs(other.attrs) {}
};