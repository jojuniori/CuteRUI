#ifndef MOEM_GUI_STATIC_TEXT_HPP
#define MOEM_GUI_STATIC_TEXT_HPP

#include <memory>
#include <string>

#include "Widget.hpp"

namespace moem {
	namespace gui {

		class StaticText : public Widget
		{
		public:
			StaticText(std::shared_ptr<Widget> parent, const std::string &label);
			virtual ~StaticText();
			void draw(float interval) override;
		protected:
			std::string label_;
		};

	}
}

#endif