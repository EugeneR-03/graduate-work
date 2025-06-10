#include "BaseWindow.hpp"
#include "TitleWindow.hpp"
#include "WindowView.hpp"

BaseWindow::BaseWindow(Size size, Position position)
    : size(size),
      size_with_margins(size),
      position(position)
{
    init();
}

BaseWindow::BaseWindow(BaseWindow::ptr parent, Size size, Position local_position)
    : parent(parent),
      size(size),
      size_with_margins(size),
      position(local_position)
{
    init();
}

BaseWindow::BaseWindow(BaseWindow::ptr parent, Size size, Margins margins)
    : parent(parent),
      size(size),
      size_with_margins(BaseWindow::calculate_size_with_margins(size, margins)),
      margins(margins)
{
    auto parent_border_length = parent->get_border_length();
    position = calculate_position_with_margins_and_borders(position, margins, Margins(parent_border_length));
    init();
}

BaseWindow::~BaseWindow()
{
    if (auto parent_ptr = parent.lock()) {
        parent_ptr->remove_child(this);
    }

    // auto children_copy = children;
    // for (auto child : children_copy) {
    //     child->del();
    // }
    
    del();
}

BaseWindow::ptr BaseWindow::create(Size size, Position position)
{
    return BaseWindow::ptr(new BaseWindow(size, position));
}

BaseWindow::ptr BaseWindow::create(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    return create(Size(height, width), Position(start_y, start_x));
}

BaseWindow::ptr BaseWindow::create(BaseWindow::ptr parent, Size size, Position local_position)
{
    if (!parent) {
        throw std::runtime_error("BaseWindow::create(): Parent window does not exist");
    }

    auto window = BaseWindow::ptr(new BaseWindow(parent, size, local_position));
    parent->add_child(window);
    return window;
}

BaseWindow::ptr BaseWindow::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    if (!parent) {
        throw std::runtime_error("BaseWindow::create(): Parent window does not exist");
    }

    auto window = BaseWindow::ptr(new BaseWindow(parent, Size(height, width), Position(start_y, start_x)));
    parent->add_child(window);
    return window;
}

BaseWindow::ptr BaseWindow::create(BaseWindow::ptr parent, Size size, Margins margins)
{
    if (!parent) {
        throw std::runtime_error("BaseWindow::create(): Parent window does not exist");
    }

    auto window = BaseWindow::ptr(new BaseWindow(parent, size, margins));
    parent->add_child(window);
    return window;
}

BaseWindow::ptr BaseWindow::create(BaseWindow::ptr parent, unsigned height, unsigned width, unsigned margin_top, unsigned margin_bottom, unsigned margin_left, unsigned margin_right)
{
    if (!parent) {
        throw std::runtime_error("BaseWindow::create(): Parent window does not exist");
    }

    auto window = BaseWindow::ptr(new BaseWindow(parent, Size(height, width), Margins(margin_top, margin_bottom, margin_left, margin_right)));
    parent->add_child(window);
    return window;
}

BaseWindow* BaseWindow::get_parent() const
{
    return parent.lock().get();
}

void BaseWindow::set_parent(BaseWindow::ptr new_parent, unsigned local_start_y, unsigned local_start_x)
{
    if (auto old_parent = this->parent.lock()) {
        old_parent->remove_child(shared_from_this());
    }

    del();
    new_parent->add_child(shared_from_this());
    parent = new_parent;
    position = { local_start_y, local_start_x };
    init();
}
 
WINDOW* BaseWindow::get_win() const { return win.get(); }

PANEL* BaseWindow::get_panel() const { return panel.get(); }

BaseWindow::State BaseWindow::get_state() const { return state; }

Size BaseWindow::get_size() const { return size; }
unsigned BaseWindow::get_width() const { return size.width; }
unsigned BaseWindow::get_height() const { return size.height; }

Position BaseWindow::get_position() const { return position; }
unsigned BaseWindow::get_start_y() const { return position.start_y; }
unsigned BaseWindow::get_start_x() const { return position.start_x; }

Margins BaseWindow::get_margins() const { return margins; }
unsigned BaseWindow::get_margins_top() const { return margins.top; }
unsigned BaseWindow::get_margins_bottom() const { return margins.bottom; }
unsigned BaseWindow::get_margins_left() const { return margins.left; }
unsigned BaseWindow::get_margins_right() const { return margins.right; }

void BaseWindow::set_margins(const Margins& margins)
{
    if (this->margins != margins) {
        this->margins = margins;
        if (get_parent()) {
            del();
            update_layout();
            init();
        }
    }
}

void BaseWindow::set_margins(unsigned top, unsigned bottom, unsigned left, unsigned right)
{
    Margins new_margins(top, bottom, left, right);
    if (this->margins != new_margins) {
        this->margins = new_margins;
        update_layout();
    }
}

std::shared_ptr<IBorder> BaseWindow::get_unfocused_border() const { return unfocused_border; }
void BaseWindow::set_unfocused_border(std::shared_ptr<IBorder> unfocused_border)
{
    if (unfocused_border && !active_border) {
        set_active_border(unfocused_border);
    }
    this->unfocused_border = unfocused_border;
}

std::shared_ptr<IBorder> BaseWindow::get_focused_border() const { return focused_border; }
void BaseWindow::set_focused_border(std::shared_ptr<IBorder> focused_border)
{
    if (focused_border && !active_border && !unfocused_border) {
        set_active_border(focused_border);
    }
    this->focused_border = focused_border;
}

std::shared_ptr<IBorder> BaseWindow::get_active_border() const { return active_border; }
void BaseWindow::set_active_border(std::shared_ptr<IBorder> active_border)
{
    bool updated = false;
    if (!this->active_border && this->active_border != active_border) {
        updated = true;
    }
    this->active_border = active_border;
    if (updated) {
        update_layout();
    }
}

void BaseWindow::resize(unsigned height, unsigned width)
{
    if (height == size.height && width == size.width) {
        return;
    }

    float height_ratio = (float)height / (float)size.height;
    float width_ratio = (float)width / (float)size.width;

    del();
    size = Size(height, width);
    size_with_margins = BaseWindow::calculate_size_with_margins(size, margins);
    init();

    for (auto child : children) {
        auto child_size = child->get_size();
        child_size.height = roundf(child_size.height * height_ratio);
        if (child_size.height > size.height) {
            child_size.height = size.height;
        }
        child_size.width = roundf(child_size.width * width_ratio);
        if (child_size.width > size.width) {
            child_size.width = size.width;
        }

        auto child_position = child->get_position();
        child_position.start_y = roundf(child_position.start_y * height_ratio);
        child_position.start_x = roundf(child_position.start_x * width_ratio);
        child->position = child_position;
        child->resize(child_size.height, child_size.width);
    }

    update_panels();
    on_resize();
}

void BaseWindow::move_to(unsigned new_y, unsigned new_x)
{
    if (new_y == position.start_y && new_x == position.start_x) {
        return;
    }

    for (auto child : children) {
        child->del();
    }
    
    if (auto parent_ptr = get_parent()) {
        int abs_y, abs_x;
        getbegyx(parent_ptr->get_win(), abs_y, abs_x);
        if (move_panel(panel.get(), abs_y + new_y, abs_x + new_x) == ERR) {
            throw std::runtime_error("BaseWindow::move_to(): Failed to move sub-panel");
        }
    }
    else {
        if (move_panel(panel.get(), new_y, new_x) == ERR) {
            throw std::runtime_error("BaseWindow::move_to(): Failed to move panel");
        }
    }

    position = { new_y, new_x };

    for (auto child : children) {
        child->init();
    }

    update_panels();
    on_move();
}

void BaseWindow::update(unsigned height, unsigned width, unsigned start_y, unsigned start_x)
{
    if (height != size.height || width != size.width) {
        resize(height, width);
    }
    if (start_y != position.start_y || start_x != position.start_x) {
        move_to(start_y, start_x);
    }
}

void BaseWindow::show()
{
    if (is_showing() && !nu::display::needs_redraw(this)) {
        return;
    }

    state |= State::Showing;

    if (active_border) {
        active_border->wdraw(win.get());
        update_panels();
    }
    show_panel(panel.get());

    update_panels();
    doupdate();

    nu::display::windows_manager.top_window_and_update(this);
    on_show();
}

void BaseWindow::hide()
{
    if (!is_showing()) {
        return;
    }

    state &= ~State::Showing;
    hide_panel(panel.get());

    update_panels();
    doupdate();

    nu::display::windows_manager.hide_window_and_update(this);
    on_hide();
}

void BaseWindow::set_focus(bool on)
{
    bool focused = is_focused();
    bool switched = false;
    if (!focused && on) {
        state |= State::Focused;
        if (active_border != focused_border) {
            switched = true;
        }
        set_active_border(focused_border);
        on_focus();
    }
    else if (focused && !on) {
        state &= ~State::Focused;
        if (active_border != unfocused_border) {
            switched = true;
        }
        set_active_border(unfocused_border);
        on_unfocus();
    }

    if (switched) {
        nu::display::mark_window(this, nu::Mark::Redraw);
    }
}

void BaseWindow::move_to_and_show(int new_y, int new_x)
{
    move_to(new_y, new_x);
    show();
}

void BaseWindow::resize_and_show(int height, int width)
{
    resize(height, width);
    show();
}

void BaseWindow::update_and_show(int height, int width, int start_y, int start_x)
{
    update(height, width, start_y, start_x);
    show();
}

bool BaseWindow::is_showing() const
{
    return has_flag(state, State::Showing);
}

bool BaseWindow::is_focused() const
{
    return has_flag(state, State::Focused);
}

unsigned BaseWindow::get_border_length() const
{
    if (active_border != nullptr) {
        return 1;
    }
    else {
        return 0;
    }
}

void BaseWindow::init()
{
    if (auto parent_ptr = get_parent()) {
        if (!win) {
            win = WINDOW_ptr(derwin(
                parent_ptr->get_win(), 
                size_with_margins.height, size_with_margins.width, 
                position.start_y, position.start_x
            ));
        }
    }
    else {
        if (!win) {
            win = WINDOW_ptr(newwin(
                size_with_margins.height, size_with_margins.width, 
                position.start_y, position.start_x
            ));
        }
    }
    
    if (!win) {
        throw std::runtime_error("BaseWindow::BaseWindow(): Failed to create window");
    }
    if (!panel) {
        panel = PANEL_ptr(new_panel(win.get()));
    }
    if (!panel) {
        throw std::runtime_error("BaseWindow::BaseWindow(): Failed to create panel");
    }

    for (auto& child : children) {
        child->init();
    }
}

void BaseWindow::del()
{
    for (auto& child : children) {
        child->hide();
        child->del();
    }
    
    panel.reset();
    win.reset();
}

void BaseWindow::update_layout()
{
    bool updated = false;

    auto parent_ptr = get_parent();
    if (parent_ptr) {
        auto parent_border_length = parent_ptr->get_border_length();
        unsigned parent_title_height = 0;

        for (auto& child : parent_ptr->get_children()) {
            if (TitleWindow* title_window = dynamic_cast<TitleWindow*>(child.get())) {
                parent_title_height = title_window->get_size().height;
                break;
            }
        }
        if (size.height >= parent_ptr->get_size().height - parent_title_height || size.width >= parent_ptr->get_size().width - parent_border_length * 2 || margins != 0) {
            Margins parent_borders;
            if (!parent_borders_considered) {
                parent_borders = Margins(parent_border_length);
            }
            else {
                parent_borders = Margins(0);
            }
            auto calc_size = BaseWindow::calculate_size_with_margins_and_borders(size, margins, parent_borders);
            if (parent_title_height > 0 && !parent_borders_considered) {
                calc_size.height += parent_border_length;
            }
            resize(calc_size.height, calc_size.width);
            updated = true;
        }
        if (position.start_y > parent_title_height || position.start_x < parent_border_length || margins != 0) {
            Margins parent_borders;
            if (!parent_borders_considered) {
                parent_borders = Margins(parent_border_length);
            }
            else {
                parent_borders = Margins(0);
            }
            auto calc_position = BaseWindow::calculate_position_with_margins_and_borders(position, margins, parent_borders);
            if (parent_title_height > 0 && !parent_borders_considered) {
                calc_position.start_y -= parent_border_length;
            }
            move_to(calc_position.start_y, calc_position.start_x);
            updated = true;
        }

        if (updated) {
            if (parent_border_length > 0) {
                parent_borders_considered = true;
            }
        }
    }

    for (auto& child : children) {
        if (dynamic_cast<TitleWindow*>(child.get())) {
            continue;
        }
        child->update_layout();
    }
}

std::vector<BaseWindow::ptr> BaseWindow::get_children() const
{
    return children;
}

void BaseWindow::add_child(BaseWindow::ptr child)
{
    children.push_back(child);
    child->parent = shared_from_this();
}

void BaseWindow::remove_child(BaseWindow::ptr child)
{
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        children.erase(it);
    }
}

void BaseWindow::remove_child(BaseWindow* child)
{
    auto it = std::find_if(
        children.begin(), children.end(), 
        [child](BaseWindow::ptr ptr) { return ptr.get() == child; }
    );
    if (it != children.end()) {
        children.erase(it);
    }
}

Size BaseWindow::calculate_size_with_margins(const Size& size, const Margins& margins)
{
    return Size(
        size.height - margins.top - margins.bottom, 
        size.width - margins.left - margins.right
    );
}

Size BaseWindow::calculate_size_with_margins_and_borders(const Size& size, const Margins& margins, Margins border_length)
{
    return Size(
        size.height - border_length.top - border_length.bottom - margins.top - margins.bottom, 
        size.width - border_length.left - border_length.right - margins.left - margins.right
    );
}

Position BaseWindow::calculate_position_with_margins(const Position& position, const Margins& margins)
{
    return Position(
        position.start_y + margins.top, 
        position.start_x + margins.left
    );
}

Position BaseWindow::calculate_position_with_margins_and_borders(const Position& position, const Margins& margins, Margins border_length)
{
    return Position(
        position.start_y + border_length.top + margins.top, 
        position.start_x + border_length.left + margins.left
    );
}