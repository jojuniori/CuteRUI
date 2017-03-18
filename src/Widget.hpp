#ifndef MOEM_GUI_WIDGET_HPP
#define MOEM_GUI_WIDGET_HPP

#include "Common.hpp"
#include <nanovg.h>
#include <memory>
#include <vector>
#include <boost/signals2/signal.hpp>

namespace moem {
namespace gui {

class Window;


/**
* \class Widget
*
* \brief Base class of all widgets.
*
* \ref Widget is the base class of all widgets in \c moem gui.
*/
class MOEM_GUI_EXPORT Widget : public std::enable_shared_from_this<Widget>
{
public:
	/// Construct a new widget with the given parent widget
	explicit Widget(std::shared_ptr<Widget> parent);
	/// Construct a new widget with the given parent widget
	explicit Widget(Window * window);
	/// Destruct a new widget with the given parent widget
	virtual ~Widget();

	/// Return the parent widget
	std::weak_ptr<Widget> parent() const;

	/// Set the parent widget
	void setParent(std::shared_ptr<Widget> parent);

	/// Return the position relative to the parent widget
	const PointType& position() const;
	/// Set the position relative to the parent widget
	void setPosition(const PointType& pos);

	/// Check if the widget contains a certain position
	bool contains(const PointType& p) const;

	/// Return the absolute position on window
	PointType absolutePosition() const;

	/// Return the size of the widget
	const SizeType& size() const;
	/// set the size of the widget
	void setSize(const SizeType& size);

	/// Return the number of child widgets
	std::size_t childCount() const;

	/// Return the list of child widgets of the current widget
	const std::vector<std::shared_ptr<Widget>>& children() const;

	/**
	* \brief Add a child widget to the current widget at
	* the specified index.
	*
	* This function almost never needs to be called by hand,
	* since the constructor of \ref Widget automatically
	* adds the current widget to its parent
	*/
	virtual void addChild(int index, std::shared_ptr<Widget> child);

	/// Convenience function which appends a widget at the end
	void addChild(std::shared_ptr<Widget> child);

	/// Remove a child widget by index
	void removeChild(int index);

	/// Remove a child widget by value
	void removeChild(std::shared_ptr<Widget> widget);

	/// Retrieves the child at the specific position
	std::shared_ptr<Widget> childAt(int index);

	/// Returns the index of a specific child or -1 if not found
	int childIndex(std::shared_ptr<Widget> widget) const;

	/// Variadic shorthand notation to construct and add a child widget
	/*template <typename WidgetClass, typename... Args>
	WidgetClass* add(const Args&... args)
	{
		return new WidgetClass(shared_from_this(), args...);
	}*/

	/// Return whether or not this widget is currently focused
	bool focused() const;
	/// Set whether or not this widget is currently focused
	void setFocused(bool focused);
	/// Request the focus to be moved to this widget
	void requestFocus();

	/// Return current font size. If not set the default of the current theme will be returned
	int fontSize() const;
	/// Set the font size of this widget
	void setFontSize(int fontSize);
	/// Return whether the font size is explicitly specified for this widget
	bool hasFontSize() const;

	NVGcolor backgroundColor();
	void setBackgroundColor(NVGcolor color);
	NVGcolor color();
	void setColor(NVGcolor color);

	/// Draw the widget (and all child widgets)
	virtual void draw(float interval);

	// event signals
	boost::signals2::signal<bool(int, int, int), once_true> mouse_button_signal;
	boost::signals2::signal<bool(double, double), once_true> cursor_pos_signal;
	boost::signals2::signal<bool(bool), once_true> curosr_enter_signal;
	boost::signals2::signal<bool(double, double), once_true> scroll_signal;
	boost::signals2::signal<bool(int, int, int, int), once_true> key_signal;
	boost::signals2::signal<bool(unsigned int), once_true> char_signal;
	boost::signals2::signal<bool(unsigned int, int), once_true> char_mod_signal;
	boost::signals2::signal<bool(const std::string&), once_true> drop_signal;
	boost::signals2::signal<bool(int, int), once_true> joystick_signal;
private:
	Widget();

protected:
	std::weak_ptr<Widget> parent_;
	Window * window_;
	NVGcontext * vg_;
	PointType position_;
	SizeType size_;
	NVGcolor color_, background_color_;
	std::vector<std::shared_ptr<Widget>> children_;
	bool focused_;
	int font_size_;
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
}
}

#endif
