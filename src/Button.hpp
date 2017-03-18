#ifndef MOEM_GUI_BUTTON_HPP
#define MOEM_GUI_BUTTON_HPP

#include <memory>
#include <string>

#include "Widget.hpp"

namespace moem {
	namespace gui {

		class Button : public Widget
		{
		public:
			Button(std::shared_ptr<Widget> parent, const std::string &label);
			virtual ~Button();
			void draw(float interval) override;
			NVGcolor hoveredColor();
			void setHoveredColor(NVGcolor color);
		protected:
			bool hover_slot(bool entered);
			std::string label_;
			NVGcolor hovered_color_;
			bool hovered_;
		};

	}
}

#endif