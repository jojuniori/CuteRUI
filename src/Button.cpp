#include "Button.hpp"

#include <functional>

namespace moem {
namespace gui {

Button::Button(std::shared_ptr<Widget> parent, const std::string& label)
	: Widget(parent), label_(label), hovered_(false), hovered_color_(nvgRGBA(0xff, 0xff, 0xff, 0x50))
{
	curosr_enter_signal.connect(std::bind(std::mem_fn(&Button::hover_slot), this, std::placeholders::_1));
}

Button::~Button() { }

void Button::draw(float interval)
{
	nvgBeginPath(vg_);
	nvgRoundedRect(vg_, position_(0), position_(1), size_(0), size_(1), 3.0f);
	nvgFillColor(vg_, hovered_ ? hoveredColor() : backgroundColor());
	nvgFill(vg_);

	nvgFontSize(vg_, fontSize());
	nvgFontFace(vg_, "sans");
	nvgTextAlign(vg_, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

	nvgFontBlur(vg_, 0);
	nvgFillColor(vg_, color());
	nvgText(vg_, position_(0) + size_(0) * 0.5, position_(1) + size_(1) * 0.5, label_.c_str(), '\0');
}


NVGcolor Button::hoveredColor()
{
	return hovered_color_;
}

void Button::setHoveredColor(NVGcolor color)
{
	hovered_color_ = color;
}

bool Button::hover_slot(bool entered)
{
	if (entered) {
		hovered_ = entered;
	}
	hovered_ = entered;
	return false;
}
}
}
