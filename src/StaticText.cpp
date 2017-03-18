#include "StaticText.hpp"

#include <functional>

namespace moem {
namespace gui {

StaticText::StaticText(std::shared_ptr<Widget> parent, const std::string& label)
	: Widget(parent), label_(label)
{
}

StaticText::~StaticText() { }

void StaticText::draw(float interval)
{
	nvgFontSize(vg_, fontSize());
	nvgFontFace(vg_, "sans");
	nvgTextAlign(vg_, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

	nvgFontBlur(vg_, 0);
	nvgFillColor(vg_, color());
	nvgText(vg_, position_(0), position_(1) + size_(1) * 0.5, label_.c_str(), '\0');
}

}
}
