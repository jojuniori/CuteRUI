#include "Window.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define NANOVG_GL2_IMPLEMENTATION
#include <nanovg.h>
#include <nanovg_gl.h>
#include "EventDispatcher.hpp"

namespace moem {
namespace gui {

class Window::GLContext
{
public:
    GLFWwindow * root_window_ = nullptr;
	void startup();
	void terminate();
	void setRootWindow(GLFWwindow * window);
};

void Window::GLContext::startup()
{
	glfwInit();
	EventDispatcher::init();
}

void Window::GLContext::terminate()
{
	glfwTerminate();
}

void Window::GLContext::setRootWindow(GLFWwindow* window)
{
	assert(root_window_ == nullptr);
	root_window_ = window;

	glfwMakeContextCurrent(window);

	if (!gladLoadGL()) {
		printf("Something went wrong!\n");
		exit(-1);
	}
}
Window::GLContext * Window::gl_context_ = new Window::GLContext();

Window::Window()
    : Window::Window("moem")
{
}

Window::Window(const std::string &title)
{
	if (gl_context_->root_window_ == nullptr) {
		gl_context_->startup();
	}
	glfwWindowHint(GLFW_DEPTH_BITS, 16);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_TRANSPARENT, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	window_ = glfwCreateWindow(270, 320, title.c_str(), 0, 0);

    if (gl_context_->root_window_ == nullptr) {
        gl_context_->setRootWindow(window_);
    } else {
		glfwMakeContextCurrent(window_);
	}
    
	vg_ = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	int fontNormal = nvgCreateFont(vg_, "sans", "C:\\Windows\\Fonts\\msyh.ttc");
	int fontSerif = nvgCreateFont(vg_, "sans", "C:\\Windows\\Fonts\\msyh.ttc");
}

Window::~Window()
{
	glfwDestroyWindow(window_);
    if (window_ == gl_context_->root_window_) {
		gl_context_->terminate();
    }
}

PointType Window::position() const
{
	PointType pos;
	int x, y;
	glfwGetWindowSize(window_, &x, &y);
	pos(0) = x;
	pos(1) = y;
	return pos;
}

void Window::setPosition(const PointType& pos)
{
	glfwSetWindowPos(window_, pos(0), pos(1));
}

SizeType Window::size() const
{
	SizeType size;
	int w, h;
	glfwGetWindowSize(window_, &w, &h);
	size(0) = w;
	size(1) = h;
	return size;
}

void Window::setSize(const SizeType& size)
{
	glfwSetWindowSize(window_, size(0), size(1));
	glViewport(0, 0, size(0), size(1));
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(window_);
}

void Window::setRootWidget(std::shared_ptr<Widget> widget)
{
	root_widget_ = widget;
	EventDispatcher::getDispatcher()->setRootWidget(window_, widget);
}

void Window::draw(float interval)
{
	glfwMakeContextCurrent(window_);
	int w = -1, h = -1;
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwGetWindowSize(window_, &w, &h);
	nvgBeginFrame(vg_, w, h, 1);
	nvgSave(vg_);
	root_widget_->draw(interval);
	nvgRestore(vg_);
	nvgEndFrame(vg_);

	glfwSwapBuffers(window_);
}

NVGcontext * Window::getNVGContext() const
{
	return vg_;
}

void Window::close()
{
	glfwSetWindowShouldClose(window_, 1);
}

GLFWwindow * Window::window() const
{
	return window_;
}

void Window::hide()
{
	glfwHideWindow(window_);
}

void Window::show()
{
	glfwShowWindow(window_);
}
}
}
