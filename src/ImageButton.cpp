#include "ImageButton.hpp"

ImageButton::ImageButton(std::shared_ptr<Widget> parent, const uint8_t * data, std::size_t len)
    : moem::gui::Button(parent, "")
{
	int iw, ih;
	image_ = nvgCreateImageMem(vg_, 0, (unsigned char *)data, len);
	nvgImageSize(vg_, image_, &iw, &ih);
	image_size_(0) = iw;
	image_size_(1) = ih;
}

ImageButton::~ImageButton()
{
}

void ImageButton::draw(float interval)
{
	auto imgPaint = nvgImagePattern(vg_, position_(0), position_(1), size_(0), size_(1), 0.0f, image_, hovered_ ? 0.8f : 1.0f);
	nvgBeginPath(vg_);
	nvgRect(vg_, position_(0), position_(1), size_(0), size_(1));
	nvgFillPaint(vg_, imgPaint);
	nvgFill(vg_);
}