#pragma once

#include "./../NcursesUtils.hpp"
#include "BaseWindow.hpp"

class TitleWindow : public BaseWindow {
public:
    using ptr = std::shared_ptr<TitleWindow>;

    static TitleWindow::ptr create(Size size, Position position, TextW title);
    static TitleWindow::ptr create(unsigned height, unsigned width, unsigned start_y, unsigned start_x, TextW title);
    static TitleWindow::ptr create(BaseWindow::ptr parent, Size size, Position local_position, TextW title);
    static TitleWindow::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x, TextW title);
    static TitleWindow::ptr create(BaseWindow::ptr parent, Size size, Margins margins, TextW title);
    static TitleWindow::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, 
                                   unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right, 
                                   TextW title);
    
    TitleWindow(const TitleWindow& other) = delete;
    TitleWindow& operator=(const TitleWindow& other) = delete;
    TitleWindow(TitleWindow&& other) = delete;
    TitleWindow& operator=(TitleWindow&& other) = delete;
    ~TitleWindow() override;

    TextW get_title() const;
    void set_title(const TextW& title);
    void print_title();
    
    virtual void resize(unsigned height, unsigned width) override;
    virtual void move_to(unsigned new_y, unsigned new_x) override;
    virtual void update(unsigned height, unsigned width, unsigned start_y, unsigned start_x) override;
    virtual void show() override;
    virtual void hide() override;

protected:
    TextW title;

    TitleWindow(Size size, Position position, TextW title);
    TitleWindow(BaseWindow::ptr parent, Size size, Position local_position, TextW title);
    TitleWindow(BaseWindow::ptr parent, Size size, Margins margins, TextW title);
};