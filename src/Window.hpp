#ifndef MOEM_GUI_WINDOW_HPP
#define MOEM_GUI_WINDOW_HPP

#include "Common.hpp"
#include <memory>

struct GLFWwindow;
struct NVGcontext;

namespace moem {
namespace gui {

class Widget;

class Window
{
public:
    Window();
	Window(const std::string &title);
    ~Window();
	/// Return the position relative to the parent widget
	PointType position() const;
	/// Set the position relative to the parent widget
	void setPosition(const PointType& pos);

	/// Return the size of the widget
	SizeType size() const;
	/// set the size of the widget
	void setSize(const SizeType& size);

	NVGcontext * getNVGContext() const;
	void setRootWidget(std::shared_ptr<Widget> widget);

	void draw(float interval);

	bool shouldClose() const;
	void close();

	GLFWwindow * window() const;
protected:
    class GLContext;
    static GLContext * gl_context_;
	GLFWwindow * window_;
	NVGcontext * vg_;
	std::shared_ptr<Widget> root_widget_;
};

}
}

#endif