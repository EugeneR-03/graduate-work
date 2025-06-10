#pragma once

#include "WindowView.hpp"
#include "./../NcursesUtils.hpp"

class WindowView;

class ButtonView : public WindowView {
public:
    using ptr = std::shared_ptr<ButtonView>;

    static ButtonView::ptr create(Size size, Position position, TextW title);
    static ButtonView::ptr create(unsigned height, unsigned width, unsigned start_y, unsigned start_x, TextW title);
    static ButtonView::ptr create(BaseWindow::ptr parent, Size size, Position local_position, TextW title);
    static ButtonView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x, TextW title);
    static ButtonView::ptr create(BaseWindow::ptr parent, Size size, Margins margins, TextW title);
    static ButtonView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right, TextW title);
    
    ButtonView(ButtonView& other) = delete;
    ButtonView& operator=(const ButtonView& other) = delete;
    ButtonView(ButtonView&& other) = delete;
    ButtonView& operator=(ButtonView&& other) = delete;
    ~ButtonView() override;

    TextW get_text();
    void set_text(TextW text);

protected:
    ButtonView(Size size, Position position, TextW title);
    ButtonView(BaseWindow::ptr parent, Size size, Position local_position, TextW title);
    ButtonView(BaseWindow::ptr parent, Size size, Margins margins, TextW title);
};