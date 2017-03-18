#ifndef MOEM_GUI_IMAGE_HPP
#define MOEM_GUI_IMAGE_HPP

#include <stdint.h>
#include "Widget.hpp"

namespace moem {
namespace gui {

class Image : public Widget
{
public:
	Image(std::shared_ptr<Widget> parent, const std::string &img);
	Image(std::shared_ptr<Widget> parent, const uint8_t * data, std::size_t len);
	virtual void draw(float interval) override;
protected:
	int image_;
	SizeType image_size_;
};

}
}

#endif