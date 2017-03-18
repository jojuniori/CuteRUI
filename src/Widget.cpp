#include "Widget.hpp"
#include <algorithm>

#include "EventDispatcher.hpp"
#include "Window.hpp"

namespace moem {
namespace gui {

Widget::Widget()
	: std::enable_shared_from_this<moem::gui::Widget>(),
	  parent_(), vg_(nullptr),
	  position_(0, 0), size_(0, 0), color_(NVGcolor{0, 0, 0, 0}),
	  background_color_(NVGcolor{0, 0, 0, 0}), focused_(false), font_size_(-1) { }

Widget::Widget(std::shared_ptr<Widget> parent) : Widget()
{
	parent_ = parent;
	vg_ = parent->vg_;
	window_ = parent->window_;
}

Widget::Widget(Window * window) : Widget()
{
	window_ = window;
	vg_ = window->getNVGContext();
}

Widget::~Widget() { }

std::weak_ptr<Widget> Widget::parent() const
{
	return parent_;
}

void Widget::setParent(std::shared_ptr<Widget> parent)
{
	parent_ = parent;
}

const PointType& Widget::position() const
{
	return position_;
}

void Widget::setPosition(const PointType& pos)
{
	position_ = pos;
}

PointType Widget::absolutePosition() const
{
	// TODO: implement
	return position_;
}

const SizeType& Widget::size() const
{
	return size_;
}

void Widget::setSize(const SizeType& size)
{
	size_ = size;
}

std::size_t Widget::childCount() const
{
	return children_.size();
}

const std::vector<std::shared_ptr<Widget>>& Widget::children() const
{
	return children_;
}

void Widget::addChild(int index, std::shared_ptr<Widget> child)
{
	children_.insert(children_.begin() + index, child);
}

void Widget::addChild(std::shared_ptr<Widget> child)
{
	children_.push_back(child);
}

void Widget::removeChild(int index)
{
	children_.erase(children_.begin() + index);
}

void Widget::removeChild(std::shared_ptr<Widget> widget)
{
	std::remove(children_.begin(), children_.end(), widget);
}

std::shared_ptr<Widget> Widget::childAt(int index)
{
	return children_[index];
}

int Widget::childIndex(std::shared_ptr<Widget> widget) const
{
	return std::find(children_.begin(), children_.end(), widget) - children_.begin();
}

bool Widget::focused() const
{
	return focused_;
}

void Widget::setFocused(bool focused)
{
	focused_ = focused;
}

void Widget::requestFocus()
{
	EventDispatcher::getDispatcher()->requestTextFocus(shared_from_this());
	focused_ = true;
}

int Widget::fontSize() const
{
	return font_size_;
}

void Widget::setFontSize(int fontSize)
{
	font_size_ = fontSize;
}

bool Widget::hasFontSize() const
{
	return font_size_ > 0;
}

bool Widget::contains(const PointType& p) const
{
	auto d = (p - position_).array();
	return (d >= 0).all() && (d < size_.array()).all();
}

NVGcolor Widget::backgroundColor()
{
	return background_color_;
}

void Widget::setBackgroundColor(NVGcolor color)
{
	background_color_ = color;
}

NVGcolor Widget::color()
{
	return color_;
}

void Widget::setColor(NVGcolor color)
{
	color_ = color;
}

void Widget::draw(float interval)
{
	nvgBeginPath(vg_);
	nvgRoundedRect(vg_, position_(0), position_(1), size_(0), size_(1), 3.0f);
	nvgFillColor(vg_, backgroundColor());
	nvgFill(vg_);

	for (auto widget : children_) {
		widget->draw(interval);
	}
}

}
}
