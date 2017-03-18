#include "EventDispatcher.hpp"

#include <iostream>

namespace moem {
namespace gui {

static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods);
static void cursor_pos_callback(GLFWwindow * window, double x, double y);
static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);
static void char_callback(GLFWwindow * window, unsigned int keypoint);
static void drop_callback(GLFWwindow * window, int count, const char ** paths);

struct EventDispatcher::WindowContext {
public:
	explicit WindowContext (std::shared_ptr<Widget> widget) : active(false), root_widget(widget), entered_widget(), text_focus()
	{
	}

	bool active;
	std::weak_ptr<Widget> root_widget;
	std::weak_ptr<Widget> entered_widget;
	std::weak_ptr<Widget> text_focus;
};

EventDispatcher * EventDispatcher::dispatcher = nullptr;

EventDispatcher* EventDispatcher::getDispatcher()
{
	return dispatcher;
}

void EventDispatcher::init()
{
	dispatcher = new EventDispatcher();
}

void EventDispatcher::setRootWidget(GLFWwindow* window, std::shared_ptr<Widget> widget)
{
	if (window_contexts_.find(window) == window_contexts_.end()) {
		window_contexts_.emplace(std::piecewise_construct,
			std::forward_as_tuple(window),
			std::forward_as_tuple(new WindowContext(widget)));
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetCursorPosCallback(window, cursor_pos_callback);
		glfwSetKeyCallback(window, key_callback);
		glfwSetCharCallback(window, char_callback);
		glfwSetDropCallback(window, drop_callback);
	}
}

void EventDispatcher::resetRootWidget(GLFWwindow* window)
{
	window_contexts_.erase(window);
}

void EventDispatcher::handleMouseButtonEvent(GLFWwindow * window, int button, int action, int mods)
{
	auto context = window_contexts_.find(window);
	if (context == window_contexts_.end()) {
		return;
	}
	auto widget = context->second->entered_widget;

	if (widget.expired()) {
		// the widget is deleted, find the new enterred widget
		widget = findEnterredWidget(context);
		context->second->entered_widget = widget;
	}

	for (; !widget.expired(); widget = widget.lock()->parent()) {
		if (widget.lock()->mouse_button_signal(button, action, mods)) {
			break;
		}
	}
}

void EventDispatcher::handleCursorPosEvent(GLFWwindow* window, double x, double y)
{
	PointType p { x, y };
	auto context = window_contexts_.find(window);
	if (context == window_contexts_.end()) {
		return;
	}
	auto widget = context->second->entered_widget;
	// generate cursor enter events
	if (!widget.expired()) {
		// generate cursor enter events (leave)
		for (; !widget.expired() && !widget.lock()->contains(p); widget = widget.lock()->parent()) {
			if (widget.lock()->curosr_enter_signal(false)) {
				break;
			}
		}
		for (; !widget.expired() && !widget.lock()->contains(p); widget = widget.lock()->parent()) {}

		if (widget.expired()) { // in my view, this code is never true
			widget = context->second->root_widget;
			widget.lock()->curosr_enter_signal(true);
		}

		std::shared_ptr<Widget> enterred, child = widget.lock();
		while (child != nullptr && widget.lock()->contains(p)) {
			enterred = child;
			child = nullptr;
			for (auto subwidget : enterred->children()) {
				if (subwidget->contains(p)) {
					child = subwidget;
					child->curosr_enter_signal(true);
					break; // TODO: when two widgets hava intersection
				}
			}
		}

		context->second->entered_widget = enterred;

	} else {
		context->second->entered_widget = findEnterredWidget(context);
	}

	bool handled = false;
	for (; !widget.expired() && !handled; widget = widget.lock()->parent()) {
		if (widget.lock()->cursor_pos_signal(x, y)) {
			handled = true;
		}
	}

	// default pos event handler
	static bool dragged = false;
	static double ox, oy;
	if (!handled) {
		if (dragged) {
			double xx, yy;
			int xpos, ypos;
			glfwGetWindowPos(window, &xpos, &ypos);
			glfwGetCursorPos(window, &xx, &yy);
			glfwSetWindowPos(window, xpos + xx - ox, ypos + yy - oy);
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_TRUE) {
			if (!dragged) {
				ox = x;
				oy = y;
				dragged = true;
			}
		}
		else {
			dragged = false;
		}
	}
}
void EventDispatcher::handleCursorEnterEvent(GLFWwindow* window, double x, double y) {}

void EventDispatcher::handleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto context = window_contexts_.find(window);
	if (context == window_contexts_.end()) {
		return;
	}
	auto widget = context->second->text_focus;

	if (widget.expired()) {
		// the widget is deleted, reset the focused widget
		widget = context->second->root_widget;
		context->second->text_focus = widget;
	}

	for (; !widget.expired(); widget = widget.lock()->parent()) {
		if (widget.lock()->key_signal(key, scancode, action, mods)) {
			break;
		}
	}
}

void EventDispatcher::handleCharEvent(GLFWwindow* window, unsigned int keypoint)
{
	auto context = window_contexts_.find(window);
	if (context == window_contexts_.end()) {
		return;
	}
	auto widget = context->second->text_focus;

	if (widget.expired()) {
		// the widget is deleted, reset the focused widget
		widget = context->second->root_widget;
		context->second->text_focus = widget;
	}

	for (; !widget.expired(); widget = widget.lock()->parent()) {
		if (widget.lock()->char_signal(keypoint)) {
			break;
		}
	}
}

void EventDispatcher::handleDropEvent(GLFWwindow* window, int count, const char ** paths)
{
	auto context = window_contexts_.find(window);
	if (context == window_contexts_.end()) {
		return;
	}
	context->second->root_widget.lock()->drop_signal(paths[0]);
}

void EventDispatcher::requestTextFocus(std::shared_ptr<Widget> widget)
{
	auto root = widget;
	for (; !root->parent().expired(); root = root->parent().lock()) {}
	for (auto pair : window_contexts_) {
		if (!pair.second->root_widget.expired() && pair.second->root_widget.lock() == root) {
			if (!pair.second->text_focus.expired()) {
				pair.second->text_focus.lock()->setFocused(false);
			}
			pair.second->text_focus = widget;
		}
	}
}


EventDispatcher::EventDispatcher() {}
EventDispatcher::~EventDispatcher() {}

std::shared_ptr<Widget> EventDispatcher::findEnterredWidget(std::map<GLFWwindow*, WindowContext*>::iterator pair)
{
	return findEnterredWidget(pair, pair->second->root_widget.lock());
}

std::shared_ptr<Widget> EventDispatcher::findEnterredWidget(std::map<GLFWwindow*, WindowContext*>::iterator pair, std::shared_ptr<Widget> enterred)
{
	if (pair->second->root_widget.expired()) {
		return nullptr;
	}
	double mx, my;
	glfwGetCursorPos(pair->first, &mx, &my);
	PointType p {mx, my};
	std::shared_ptr<Widget> child;

	child = enterred->contains(p) ? enterred : nullptr;

	while (child != nullptr) {
		enterred = child;
		child = nullptr;
		for (auto subwidget : enterred->children()) {
			if (subwidget->contains(p)) {
				child = subwidget;
				break; // TODO: when two widgets hava intersection
			}
		}
	}

	return enterred;
}

static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	auto dispatcher = EventDispatcher::getDispatcher();
	dispatcher->handleMouseButtonEvent(window, button, action, mods);
}

static void cursor_pos_callback(GLFWwindow * window, double x, double y)
{
	auto dispatcher = EventDispatcher::getDispatcher();
	dispatcher->handleCursorPosEvent(window, x, y);
}

static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	auto dispatcher = EventDispatcher::getDispatcher();
	dispatcher->handleKeyEvent(window, key, scancode, action, mods);
}

static void char_callback(GLFWwindow * window, unsigned int keypoint)
{
	auto dispatcher = EventDispatcher::getDispatcher();
	dispatcher->handleCharEvent(window, keypoint);
}


static void drop_callback(GLFWwindow * window, int count, const char ** paths)
{
	auto dispatcher = EventDispatcher::getDispatcher();
	dispatcher->handleDropEvent(window, count, paths);
}

}
}
