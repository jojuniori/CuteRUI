#include "Image.hpp"

namespace moem {
namespace gui {


Image::Image(std::shared_ptr<Widget> parent, const std::string& img)
	: Widget(parent)
{
	int iw, ih;
	image_ = nvgCreateImage(vg_, img.c_str(), 0);
	nvgImageSize(vg_, image_, &iw, &ih);
	image_size_(0) = iw;
	image_size_(1) = ih;
}


Image::Image(std::shared_ptr<Widget> parent, const uint8_t * data, std::size_t len)
	: Widget(parent)
{
	int iw, ih;
	image_ = nvgCreateImageMem(vg_, 0, (unsigned char *)data, len);
	nvgImageSize(vg_, image_, &iw, &ih);
	image_size_(0) = iw;
	image_size_(1) = ih;
}

void Image::draw(float interval)
{
	auto imgPaint = nvgImagePattern(vg_, position_(0), position_(1), size_(0), size_(1), 0.0f, image_, 1.0f);
	nvgBeginPath(vg_);
	nvgRect(vg_, position_(0), position_(1), size_(0), size_(1));
	nvgFillPaint(vg_, imgPaint);
	nvgFill(vg_);
	for (auto widget : children_) {
		widget->draw(interval);
	}
}

}
}