#pragma once

#include <optional>
#include "BaseWindow.hpp"
#include "TitleWindow.hpp"

class BaseWindow;

class WindowView : public BaseWindow {
public:
    using ptr = std::shared_ptr<WindowView>;

    static WindowView::ptr create(Size size, Position position);
    static WindowView::ptr create(unsigned height, unsigned width, unsigned start_y, unsigned start_x);
    static WindowView::ptr create(Size size, Position position, 
                                  TextW title, unsigned title_height);
    static WindowView::ptr create(unsigned height, unsigned width, unsigned start_y, unsigned start_x, 
                                  TextW title, unsigned title_height);
    static WindowView::ptr create(BaseWindow::ptr parent, Size size, Position local_position = Position(0, 0));
    static WindowView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y = 0, unsigned start_x = 0);
    static WindowView::ptr create(BaseWindow::ptr parent, Size size, Margins margins = Margins(0, 0, 0, 0));
    static WindowView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top = 0, unsigned margin_bottom = 0, unsigned margin_left = 0, unsigned margin_right = 0);
    static WindowView::ptr create(BaseWindow::ptr parent, Size size, Position local_position, 
                                  TextW title, unsigned title_height);
    static WindowView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x, 
                                  TextW title, unsigned title_height);
    static WindowView::ptr create(BaseWindow::ptr parent, Size size, Margins margins, 
                                  TextW title, unsigned title_height);
    static WindowView::ptr create(BaseWindow::ptr parent, unsigned height, unsigned width, 
                                  unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right, 
                                  TextW title, unsigned title_height);
    
    WindowView(WindowView& other) = delete;
    WindowView& operator=(const WindowView& other) = delete;
    WindowView(WindowView&& other) = delete;
    WindowView& operator=(WindowView&& other) = delete;
    ~WindowView();

    std::optional<TextW> get_title() const;
    std::optional<Size> get_title_size() const;
    unsigned get_title_height() const;
    unsigned get_title_width() const;
    void set_title(const TextW& title);

    void resize(unsigned height, unsigned width) override;
    void move_to(unsigned new_y, unsigned new_x) override;
    void update(unsigned height, unsigned width, unsigned start_y, unsigned start_x) override;
    void show() override;
    void hide() override;

    virtual void set_default_borders();
    virtual void set_default_event_handlers();

    static Size calculate_size_for_inner_window(Size size, Margins margins, unsigned title_height = 0, Margins border_length = Margins(0, 0, 0, 0));
    static Position calculate_position_for_inner_window(Position position, Margins margins, unsigned title_height = 0, Margins border_length = Margins(0, 0, 0, 0));

protected:
    TitleWindow::ptr title_window;

    WindowView(Size size, Position position);
    WindowView(Size size, Position position, TextW title, unsigned title_height);
    WindowView(BaseWindow::ptr parent, Size size, Position local_position);
    WindowView(BaseWindow::ptr parent, Size size, Margins margins);
    WindowView(BaseWindow::ptr parent, Size size, Position local_position, TextW title, unsigned title_height);
    WindowView(BaseWindow::ptr parent, Size size, Margins margins, TextW title, unsigned title_height);

    void init_title_window(const TextW& title, unsigned title_height);
    Size calculate_size_without_title() const;
    Position calculate_absolute_position_without_title() const;
    Position calculate_local_position_without_title() const;
    Position calculate_local_position_without_title_with_border() const;
};