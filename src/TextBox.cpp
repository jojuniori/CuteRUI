#include "TextBox.hpp"
#include "Window.hpp"

#include <regex>
#include <iostream>
#include <codecvt>

#include <GLFW/glfw3.h>

using namespace moem::gui;

static int UTF8Length(int pos, const std::string & mValueTemp)
{
	uint8_t lead = static_cast<uint8_t>(mValueTemp[pos]);
	if (lead < 0x80)
		return 1;
	else if ((lead >> 5) == 0x6)
		return 2;
	else if ((lead >> 4) == 0xe)
		return 3;
	else if ((lead >> 3) == 0x1e)
		return 4;
	else
		return 0;
}

static int UTF8Next(int pos, const std::string & mValueTemp)
{
	return std::min(pos + UTF8Length(pos, mValueTemp), (int)mValueTemp.length());
}

static int UTF8Advance(int pos, int n, const std::string & mValueTemp)
{
	for (auto len = mValueTemp.length(); n > 0 && pos < len; n--) {
		pos = UTF8Next(pos, mValueTemp);
	}

	return pos;
}

static int UTF8Prior(int pos, const std::string & mValueTemp)
{
	--pos;
	while (pos >= 0 && static_cast<uint8_t>(mValueTemp[pos]) >> 6 == 0x2) {
		--pos;
	}
	return pos;
}

static int UTF8Distance(int begin, int end, std::string & mValueTemp)
{
	int pos = begin, cnt = 0;
	while (pos < end) {
		pos = UTF8Next(pos, mValueTemp);
		++cnt;
	}
	return cnt;
}

static int moveCursorUTF8(int prev, int delta, std::string & mValueTemp)
{
	auto pos = prev;
	if (delta > 0) {
		for (auto len = mValueTemp.length(); delta > 0 && pos < len; delta--) {
			pos = UTF8Next(pos, mValueTemp);
		}
	}
	else {
		for (; delta < 0 && pos >= 0; delta++) {
			pos = UTF8Prior(pos, mValueTemp);
		}
	}
	return pos;
}

TextBox::TextBox(std::shared_ptr<Widget> parent, const std::string &value)
	: Widget(parent),
	mValue(value),
	mDefaultValue(""),
	mValueTemp(value),
	mCursorPos(-1),
	mSelectionPos(-1),
	mMousePos(PointType(-1, -1)),
	mMouseDownPos(PointType(-1, -1)),
	mMouseDragPos(PointType(-1, -1)),
	mMouseDownModifier(0),
	mTextOffset(0),
	mLastClick(0) {
    //curosr_enter_signal.connect(std::bind(std::mem_fn(&Button::hover_slot), this, std::placeholders::_1));
    mouse_button_signal.connect(std::bind(std::mem_fn(&TextBox::mouseButtonSlot), this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    cursor_pos_signal.connect(std::bind(std::mem_fn(&TextBox::cursorPosSlot), this, std::placeholders::_1, std::placeholders::_2));
    key_signal.connect(std::bind(std::mem_fn(&TextBox::keySlot), this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    char_signal.connect(std::bind(std::mem_fn(&TextBox::charSlot), this, std::placeholders::_1));
    focusEvent(false);
}

void TextBox::draw(float interval) {
    auto ctx = vg_;

    // Background
	nvgBeginPath(ctx);
	nvgRoundedRect(ctx, position_.x() + 1, position_.y() + 1 + 1.0f, size_.x() - 2, size_.y() - 2, 5);
	nvgFillColor(ctx, backgroundColor());
	nvgFill(ctx);

//	nvgBeginPath(ctx);
//	nvgRoundedRect(ctx, mPos.x() + 0.5f, mPos.y() + 0.5f, mSize.x() - 1,
//		mSize.y() - 1, 2.5f);
//	nvgStrokeColor(ctx, Color(0, 48));
//	nvgStroke(ctx);
//
	nvgFontSize(ctx, fontSize());
	nvgFontFace(ctx, "sans");
	PointType drawPos(position_.x(), position_.y() + size_.y() * 0.5f + 1);

	float xSpacing = size_.y() * 0.3f;



	nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	drawPos.x() += size_.x() * 0.5f;

	nvgFontSize(ctx, fontSize());



	if (!focused() && mValueTemp.length() == 0) {
		nvgFillColor(ctx, hintColor());
		nvgText(ctx, drawPos.x(), drawPos.y(), hint_.c_str(), nullptr);
		return;
	}

	nvgFillColor(ctx, color());

	// clip visible text area
	float clipX = position_.x() + xSpacing - 1.0f;
	float clipY = position_.y() + 1.0f;
	float clipWidth = size_.x() - 2 * xSpacing + 2.0f;
	float clipHeight = size_.y() - 3.0f;

	nvgSave(ctx);
	nvgIntersectScissor(ctx, clipX, clipY, clipWidth, clipHeight);

	PointType oldDrawPos(drawPos);
	drawPos.x() += mTextOffset;

	if (mCommitted) {
		nvgText(ctx, drawPos.x(), drawPos.y(), mValue.c_str(), nullptr);
	}
	else {
		const int maxGlyphs = 1024;
		NVGglyphPosition glyphs[maxGlyphs];
		float textBound[4];
		nvgTextBounds(ctx, drawPos.x(), drawPos.y(), mValueTemp.c_str(),
			nullptr, textBound);
		float lineh = textBound[3] - textBound[1];

		// find cursor positions
		int nglyphs =
			nvgTextGlyphPositions(ctx, drawPos.x(), drawPos.y(),
				mValueTemp.c_str(), nullptr, glyphs, maxGlyphs);
		updateCursor(ctx, textBound[2], glyphs, nglyphs);

		// compute text offset
		int CPOS = UTF8Distance(0, mCursorPos, mValueTemp);
		int prevCPos = CPOS > 0 ? CPOS - 1 : 0;
		int nextCPos = CPOS < nglyphs ? CPOS + 1 : nglyphs;
		float prevCX = cursorIndex2Position(prevCPos, textBound[2], glyphs, nglyphs);
		float nextCX = cursorIndex2Position(nextCPos, textBound[2], glyphs, nglyphs);

		if (nextCX > clipX + clipWidth)
			mTextOffset -= nextCX - (clipX + clipWidth) + 1;
		if (prevCX < clipX)
			mTextOffset += clipX - prevCX + 1;

		drawPos.x() = oldDrawPos.x() + mTextOffset;

		// draw text with offset
		nvgText(ctx, drawPos.x(), drawPos.y(), mValueTemp.c_str(), nullptr);
		nvgTextBounds(ctx, drawPos.x(), drawPos.y(), mValueTemp.c_str(),
			nullptr, textBound);

		// recompute cursor positions
		nglyphs = nvgTextGlyphPositions(ctx, drawPos.x(), drawPos.y(),
			mValueTemp.c_str(), nullptr, glyphs, maxGlyphs);

		if (mCursorPos > -1) {
			if (mSelectionPos > -1) {
				float caretx = cursorIndex2Position(UTF8Distance(0, mCursorPos, mValueTemp), textBound[2],
					glyphs, nglyphs);
				float selx = cursorIndex2Position(UTF8Distance(0, mSelectionPos, mValueTemp), textBound[2],
					glyphs, nglyphs);

				if (caretx > selx)
					std::swap(caretx, selx);

				// draw selection
				nvgBeginPath(ctx);
				nvgFillColor(ctx, nvgRGBA(255, 255, 255, 80));
				nvgRect(ctx, caretx, drawPos.y() - lineh * 0.5f, selx - caretx,
					lineh);
				nvgFill(ctx);
			}

			float caretx = cursorIndex2Position(UTF8Distance(0, mCursorPos, mValueTemp), textBound[2], glyphs, nglyphs);

			// draw cursor
			nvgBeginPath(ctx);
			nvgMoveTo(ctx, caretx, drawPos.y() - lineh * 0.5f);
			nvgLineTo(ctx, caretx, drawPos.y() + lineh * 0.5f);
			nvgStrokeColor(ctx, nvgRGBA(0x44, 0x44, 0x44, 0xff));
			nvgStrokeWidth(ctx, 1.0f);
			nvgStroke(ctx);
		}
	}
	nvgRestore(ctx);
}

bool TextBox::mouseButtonSlot(int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS && !focused_) {
        focusEvent(true);
        requestFocus();
	}

	if (focused()) {
		if (action == GLFW_PRESS) {
			mMouseDownPos = mMousePos; // todo: Check Correctness
			mMouseDownModifier = mods;

			double time = glfwGetTime();
			if (time - mLastClick < 0.25) {
				/* Double-click: select all text */
				mSelectionPos = 0;
				mCursorPos = (int)mValueTemp.size();
				mMouseDownPos = PointType{-1, -1};
			}
			mLastClick = time;
		}
		else {
			mMouseDownPos = PointType{-1, -1};
			mMouseDragPos = PointType{-1, -1};
		}
		return true;
	}

	return false;
}

bool TextBox::cursorPosSlot(double x, double y) {
	mMousePos = PointType {x, y};
    if (mMouseDownPos.x() != -1) {
        //
    }
	return true;
}
//
//bool TextBox::mouseDragEvent(const Vector2i &p, const Vector2i &/* rel */,
//	int /* button */, int /* modifiers */) {
//	mMousePos = p;
//	mMouseDragPos = p;
//
//	if (mEditable && focused()) {
//		return true;
//	}
//	return false;
//}
//
bool TextBox::focusEvent(bool focused) {
	std::string backup = mValue;

    if (focused) {
        mValueTemp = mValue;
        mCommitted = false;
        mCursorPos = 0;
    }
    else {
        if (mCallback && !mCallback(mValue))
            mValue = backup;

        mCommitted = true;
        mCursorPos = -1;
        mSelectionPos = -1;
        mTextOffset = 0;
    }

	return true;
}




bool TextBox::keySlot(int key, int scancode, int action, int modifiers) {
	if (focused()) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			if (key == GLFW_KEY_LEFT) {
				if (modifiers == GLFW_MOD_SHIFT) {
					if (mSelectionPos == -1)
						mSelectionPos = mCursorPos;
				}
				else {
					mSelectionPos = -1;
				}

				if (mCursorPos > 0)
					mCursorPos = moveCursorUTF8(mCursorPos, -1, mValueTemp);
			}
			else if (key == GLFW_KEY_RIGHT) {
				if (modifiers == GLFW_MOD_SHIFT) {
					if (mSelectionPos == -1)
						mSelectionPos = mCursorPos;
				}
				else {
					mSelectionPos = -1;
				}

				if (mCursorPos < (int)mValueTemp.length())
					mCursorPos = moveCursorUTF8(mCursorPos, 1, mValueTemp);
			}
			else if (key == GLFW_KEY_HOME) {
				if (modifiers == GLFW_MOD_SHIFT) {
					if (mSelectionPos == -1)
						mSelectionPos = mCursorPos;
				}
				else {
					mSelectionPos = -1;
				}

				mCursorPos = 0;
			}
			else if (key == GLFW_KEY_END) {
				if (modifiers == GLFW_MOD_SHIFT) {
					if (mSelectionPos == -1)
						mSelectionPos = mCursorPos;
				}
				else {
					mSelectionPos = -1;
				}

				mCursorPos = (int)mValueTemp.size();
			}
			else if (key == GLFW_KEY_BACKSPACE) {
				if (!deleteSelection()) {
					if (mCursorPos > 0) {
						int prevPos = moveCursorUTF8(mCursorPos, -1, mValueTemp);
						mValueTemp.erase(mValueTemp.begin() + prevPos, mValueTemp.begin() + mCursorPos);
						mCursorPos = prevPos;
					}
				}
			}
			else if (key == GLFW_KEY_DELETE) {
				if (!deleteSelection()) {
					if (mCursorPos < (int)mValueTemp.length()) {
						int nextPos = moveCursorUTF8(mCursorPos, 1, mValueTemp);
						mValueTemp.erase(mValueTemp.begin() + mCursorPos, mValueTemp.begin() + nextPos);
					}
				}
			}
			else if (key == GLFW_KEY_ENTER) {
				if (!mCommitted)
					focusEvent(false);
			}
			else if (key == GLFW_KEY_A && modifiers == GLFW_MOD_CONTROL) {
				mCursorPos = (int)mValueTemp.length();
				mSelectionPos = 0;
			}
			else if (key == GLFW_KEY_X && modifiers == GLFW_MOD_CONTROL) {
				copySelection();
				deleteSelection();
			}
			else if (key == GLFW_KEY_C && modifiers == GLFW_MOD_CONTROL) {
				copySelection();
			}
			else if (key == GLFW_KEY_V && modifiers == GLFW_MOD_CONTROL) {
				deleteSelection();
				pasteFromClipboard();
			}
		}

		return true;
	}

	return false;
}


bool TextBox::charSlot(unsigned int codepoint) {
	if (focused()) {
		std::string convert;
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convertor;
		convert = convertor.to_bytes(codepoint);
		deleteSelection();
		mValueTemp.insert(mCursorPos, convert);
		mCursorPos = moveCursorUTF8(mCursorPos, 1, mValueTemp);

		return true;
	}

	return false;
}

bool TextBox::copySelection() {
	if (mSelectionPos > -1) {
		//Screen *sc = dynamic_cast<Screen *>(this->window()->parent());

		int begin = mCursorPos;
		int end = mSelectionPos;

		if (begin > end)
			std::swap(begin, end);

		glfwSetClipboardString(window_->window(), mValueTemp.substr(begin, end).c_str());
		return true;
	}

	return false;
}

void TextBox::pasteFromClipboard() {
	const char* cbstr = glfwGetClipboardString(window_->window());
	if (cbstr)
		mValueTemp.insert(mCursorPos, std::string(cbstr));
}

bool TextBox::deleteSelection() {
	if (mSelectionPos > -1) {
		int begin = mCursorPos;
		int end = mSelectionPos;

		if (begin > end)
			std::swap(begin, end);

		if (begin == end - 1)
			mValueTemp.erase(mValueTemp.begin() + begin);
		else
			mValueTemp.erase(mValueTemp.begin() + begin,
				mValueTemp.begin() + end);

		mCursorPos = begin;
		mSelectionPos = -1;
		return true;
	}

	return false;
}

void TextBox::updateCursor(NVGcontext *, float lastx,
	const NVGglyphPosition *glyphs, int size) {
	// handle mouse cursor events
	if (mMouseDownPos.x() != -1) {
		if (mMouseDownModifier == GLFW_MOD_SHIFT) {
			if (mSelectionPos == -1)
				mSelectionPos = mCursorPos;
		}
		else
			mSelectionPos = -1;

		mCursorPos = UTF8Advance(0, position2CursorIndex(mMouseDownPos.x(), lastx, glyphs, size), mValueTemp);


		mMouseDownPos = PointType{-1, -1};
	}
	else if (mMouseDragPos.x() != -1) {
		if (mSelectionPos == -1)
			mSelectionPos = mCursorPos;

		mCursorPos = UTF8Advance(0, position2CursorIndex(mMouseDragPos.x(), lastx, glyphs, size), mValueTemp);
	}
	else {
		// set cursor to last character
		if (mCursorPos == -2)
			mCursorPos = size;
	}

	if (mCursorPos == mSelectionPos)
		mSelectionPos = -1;
}

float TextBox::cursorIndex2Position(int index, float lastx,
	const NVGglyphPosition *glyphs, int size) {
	float pos = 0;
	if (index == size)
		pos = lastx; // last character
	else
		pos = glyphs[index].x;

	return pos;
}

int TextBox::position2CursorIndex(float posx, float lastx,
	const NVGglyphPosition *glyphs, int size) {
	int mCursorId = 0;
	float caretx = glyphs[mCursorId].x;
	for (int j = 1; j < size; j++) {
		if (std::abs(caretx - posx) > std::abs(glyphs[j].x - posx)) {
			mCursorId = j;
			caretx = glyphs[mCursorId].x;
		}
	}
	if (std::abs(caretx - posx) > std::abs(lastx - posx))
		mCursorId = size;

	return mCursorId;
}
