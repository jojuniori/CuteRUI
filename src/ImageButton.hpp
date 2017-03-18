#ifndef MW_IMAGE_BUTTON_HPP
#define MW_IMAGE_BUTTON_HPP

#include "Button.hpp"
#include <string>

class ImageButton : public moem::gui::Button {
public:
    ImageButton(std::shared_ptr<Widget> parent, const uint8_t * data, std::size_t len);
    virtual ~ImageButton();
	virtual void draw(float interval) override;
protected:
	int image_;
	moem::gui::SizeType image_size_;
};

#endif