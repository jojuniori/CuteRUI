#ifndef MW_TEXT_BOX_HPP
#define MW_TEXT_BOX_HPP

#include "Widget.hpp"
#include <string>

class TextBox : public moem::gui::Widget {
public:
    TextBox(std::shared_ptr<moem::gui::Widget> parent, const std::string &value);

    /// How to align the text in the text box.
    enum class Alignment {
        Left,
        Center,
        Right
    };

    const std::string &value() const { return mValueTemp; }
    void setValue(const std::string &value) { mValue = value; }

	const std::string &hint() const { return hint_; }
	void setHint(const std::string &hint) { hint_ = hint; }

	const NVGcolor hintColor() const { return hintColor_; }
	void setHintColor(NVGcolor hintColor) { hintColor_ = hintColor; }

    /// Set the change callback
    std::function<bool(const std::string& str)> callback() const { return mCallback; }
    void setCallback(const std::function<bool(const std::string& str)> &callback) { mCallback = callback; }

    bool mouseButtonSlot(int button, int action, int mods);
    bool cursorPosSlot(double x, double y);
    bool keySlot(int key, int scancode, int action, int modifiers);
    bool charSlot(unsigned int codepoint);
    bool focusEvent(bool focused);

    virtual void draw(float interval);
protected:
    bool copySelection();
    void pasteFromClipboard();
    bool deleteSelection();

    void updateCursor(NVGcontext *ctx, float lastx,
                      const NVGglyphPosition *glyphs, int size);
    float cursorIndex2Position(int index, float lastx,
                               const NVGglyphPosition *glyphs, int size);
    int position2CursorIndex(float posx, float lastx,
                             const NVGglyphPosition *glyphs, int size);

protected:
    std::string mValue, hint_;
    std::string mDefaultValue;
    bool mCommitted;
    std::function<bool(const std::string& str)> mCallback;
    std::string mValueTemp;
    int mCursorPos;
    int mSelectionPos;
    moem::gui::PointType mMousePos;
    moem::gui::PointType mMouseDownPos;
    moem::gui::PointType mMouseDragPos;
    int mMouseDownModifier;
    float mTextOffset;
    double mLastClick;
	NVGcolor hintColor_;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif
