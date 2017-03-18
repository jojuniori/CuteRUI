#ifndef MOEM_GUI_EVENT_DISPATCHER_HPP
#define MOEM_GUI_EVENT_DISPATCHER_HPP

#include <memory>
#include <map>

#include <GLFW/glfw3.h>

#include "Widget.hpp"

namespace moem {
namespace gui {

class EventDispatcher
{
public:
	static EventDispatcher * getDispatcher();
	static void init();
	void setRootWidget(GLFWwindow * window, std::shared_ptr<Widget> widget);
	void resetRootWidget(GLFWwindow * window);

	inline void handleMouseButtonEvent(GLFWwindow * window, int button, int action, int mods);
	inline void handleCursorPosEvent(GLFWwindow * window, double x, double y);
	inline void handleCursorEnterEvent(GLFWwindow * window, double x, double y);

	inline void handleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	inline void handleCharEvent(GLFWwindow* window, unsigned int keypoint);
	inline void handleDropEvent(GLFWwindow* window, int count, const char ** paths);

	void requestTextFocus(std::shared_ptr<Widget> widget);
	/*
	boost::signals2::signal<bool(double, double), once_true> scroll_signal;
	boost::signals2::signal<bool(unsigned int, int), once_true> char_mod_signal;
	boost::signals2::signal<bool(const std::string&), once_true> drop_signal;
	boost::signals2::signal<bool(int, int), once_true> joystick_signal;*/
private:
	EventDispatcher();
	virtual ~EventDispatcher();
	static EventDispatcher * dispatcher;
	struct WindowContext;
	std::map<GLFWwindow *, WindowContext *> window_contexts_;
	static std::shared_ptr<Widget> findEnterredWidget(std::map<GLFWwindow *, WindowContext *>::iterator pair);
	static std::shared_ptr<Widget> findEnterredWidget(std::map<GLFWwindow *, WindowContext *>::iterator pair, std::shared_ptr<Widget> root);
};

}
}

#endif