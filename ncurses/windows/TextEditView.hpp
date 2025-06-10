#pragma once

#include "TextEditSubwindow.hpp"
#include "WindowView.hpp"

class TextEditView : public WindowView, public IRunnable {
public:
    using ptr = std::shared_ptr<TextEditView>;

    // Фабричные методы для создания TextEditView (аналогично MenuView::create).
    static ptr create(Size size, Position position);
    static ptr create(unsigned height, unsigned width, unsigned start_y, unsigned start_x);
    static ptr create(Size size, Position position, TextW title, unsigned title_height);
    static ptr create(unsigned height, unsigned width, unsigned start_y, unsigned start_x,
                      TextW title, unsigned title_height);
    static ptr create(BaseWindow::ptr parent, Size size, Position local_position);
    static ptr create(BaseWindow::ptr parent, unsigned height, unsigned width,
                      unsigned start_y, unsigned start_x);
    static ptr create(BaseWindow::ptr parent, Size size, Position local_position,
                      TextW title, unsigned title_height);
    static ptr create(BaseWindow::ptr parent, unsigned height, unsigned width,
                      unsigned start_y, unsigned start_x, TextW title, unsigned title_height);
    static ptr create(BaseWindow::ptr parent, Size size, Margins margins);
    static ptr create(BaseWindow::ptr parent, Size size, Margins margins,
                      TextW title, unsigned title_height);
    static ptr create(BaseWindow::ptr parent, unsigned height, unsigned width,
                      unsigned margin_top, unsigned margin_bottom,
                      unsigned margin_left, unsigned margin_right,
                      TextW title, unsigned title_height);
                    
    TextEditView(const TextEditView& other) = delete;
    TextEditView& operator=(const TextEditView& other) = delete;
    TextEditView(TextEditView&& other) = delete;
    TextEditView& operator=(TextEditView&& other) = delete;
    ~TextEditView() override;

    void load(const std::vector<std::string>& lines);
    std::vector<std::string> get_lines() const;

    void resize(unsigned height, unsigned width) override;
    void move_to(unsigned new_y, unsigned new_x) override;
    void update(unsigned height, unsigned width, unsigned start_y, unsigned start_x) override;
    void show() override;
    void hide() override;

    void run() override;
    void run_non_editable();
    void stop() override;
    bool is_running() override;

    void add_exit_callback(std::function<void()> callback = nullptr, std::vector<wint_t> keys = {});

protected:
    TextEditSubwindow::ptr text_window;
    
    TextEditView(Size size, Position position);
    TextEditView(Size size, Position position, TextW title, unsigned title_height);
    TextEditView(BaseWindow::ptr parent, Size size, Position position);
    TextEditView(BaseWindow::ptr parent, Size size, Margins margins);
    TextEditView(BaseWindow::ptr parent, Size size, Position position, TextW title, unsigned title_height);
    TextEditView(BaseWindow::ptr parent, Size size, Margins margins, TextW title, unsigned title_height);
};