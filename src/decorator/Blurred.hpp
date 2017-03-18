#ifndef MOEM_GUI_BLURRED_HPP
#define MOEM_GUI_BLURRED_HPP

#include "../Widget.hpp"

namespace moem {
namespace gui {
namespace decorator {

template <class widget>
class Blurred : public widget
{
public:
	template <typename... Args>
	Blurred(const Args&... args)
		: widget(args...)
	{
	}

	virtual void draw(float interval) {
		auto shadowPaint = nvgBoxGradient(this->vg_, this->position()(0), this->position()(1) + 2,
			this->size()(0), this->size()(1), 6.0f, 10, nvgRGBA(0, 0, 0, 128), nvgRGBA(0, 0, 0, 0));
		nvgBeginPath(this->vg_);
		nvgRect(this->vg_, this->position()(0) - 10, this->position()(1) - 10, this->size()(0) + 20, this->size()(1) + 20);
		nvgRoundedRect(this->vg_, this->position()(0), this->position()(1), this->size()(0), this->size()(1), 2);
		nvgPathWinding(this->vg_, NVG_HOLE);
		nvgFillPaint(this->vg_, shadowPaint);
		nvgFill(this->vg_);
		widget::draw(interval);
	}
};

}
}
}

#endif