#include <iostream>
#include <future>
#include <locale>
#include <codecvt>
#ifdef _WIN32
# include <Windows.h>
# include <direct.h>
#else
# include <stdio.h>
#endif
#include "Window.hpp"
#include "StaticText.hpp"
#include "Image.hpp"
#include "decorator/Blurred.hpp"
#include "ImageButton.hpp"
#include "TextLoader.hpp"
#include "TextBox.hpp"
#include "DominantColor.hpp"

#include <GLFW/glfw3.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>

#include "../ext/nanovg/stb_image.h"

extern "C" {
#include "../resource/resource.h"
}

using namespace boost::property_tree;

using namespace moem::gui;


class AboutDialog
{
public:
	AboutDialog()
	{
	}

	bool main_loop(float interval)
	{
		window_.draw(interval);
		return window_.shouldClose();
	}

	void init()
	{
		auto video = glfwGetVideoMode(glfwGetPrimaryMonitor());
		window_.setPosition(PointType{ (video->width - window_.size()(0)) * 0.5, (video->height - window_.size()(1)) * 0.5 });

		// main window
		window_.setSize(SizeType{ 424.0f, 374.0f });
		// root panel
		auto root_panel_ = std::make_shared<Widget>(&window_);
		root_panel_->setPosition(PointType{ 0.0f, 0.0f });
		root_panel_->setSize(SizeType{ 424.0f, 374.0f });
		//root_panel_->setBackgroundColor(nvgRGBA(0xff, 0xff, 0xff, 0xff));
		window_.setRootWidget(root_panel_);

		// backgroud image
		auto backgroud = std::make_shared<Image>(root_panel_, about_png, about_png_size);
		root_panel_->addChild(backgroud);
		backgroud->setPosition(PointType{ 0.0f, 0.0f });
		backgroud->setSize(SizeType{ 424.0f, 374.0f });

		//424,374
		float y = 20.0f;
		// close button
		auto close_button_ = std::make_shared<ImageButton>(backgroud, close_png, close_png_size);
		backgroud->addChild(close_button_);
		close_button_->setPosition(PointType{ 377.0f, 20.0f });
		close_button_->setSize(SizeType{ 16.0f, 16.0f });
		close_button_->setBackgroundColor(nvgRGBA(0xf4, 0x65, 0x59, 0xff));
		close_button_->setHoveredColor(nvgRGBA(0xf4, 0x65, 0x59, 0xcc));
		close_button_->setColor(nvgRGBA(0xff, 0xff, 0xff, 0xff));
		close_button_->setFontSize(12);

		close_button_->mouse_button_signal.connect([this](int button, int action, int mods)
		{
			if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
				this->hide();
			}
			return false;
		});
		// btn
		auto cuter = std::make_shared<Button>(backgroud, "");
		backgroud->addChild(cuter);
		cuter->setPosition(PointType{ 302.0f, 196.0f });
		cuter->setSize(SizeType{ 41.0f, 14.0f });
		cuter->mouse_button_signal.connect([this](int button, int action, int mods)
		{
			if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
				this->open(L"https://github.com/chinuno-usami/CuteR");
			}
			return false;
		});
		// btn
		auto source = std::make_shared<Button>(backgroud, "");
		backgroud->addChild(source);
		source->setPosition(PointType{ 166.0f, 277.0f });
		source->setSize(SizeType{ 44.0f, 14.0f });
		source->mouse_button_signal.connect([this](int button, int action, int mods)
		{
			if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
				this->open(L"https://github.com/jojuniori/CuteRUI");
			}
			return false;
		});
		// btn
		auto moem = std::make_shared<Button>(backgroud, "");
		backgroud->addChild(moem);
		moem->setPosition(PointType{ 255.0f, 277.0f });
		moem->setSize(SizeType{ 96.0f, 14.0f });
		moem->mouse_button_signal.connect([this](int button, int action, int mods)
		{
			if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
				this->open(L"http://www.moem.cc");
			}
			return false;
		});
	}

	void hide()
	{
		window_.hide();
	}

	void show()
	{
		window_.show();
	}

	void open(const std::wstring& url)
	{
		ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

protected:
	Window window_;
};

class CuteRuiDialog
{
public:
	CuteRuiDialog()
		: about_(nullptr)
	{
	}

	void main_loop()
	{
		auto time_pre = glfwGetTime();
		/* Loop until the user closes the window */
		while (!window_.shouldClose())
		{
			auto time = glfwGetTime();
			auto interval = time - time_pre;
			window_.draw(interval);
			if (about_ != nullptr) {
				(about_->main_loop(interval));
			}
			time_pre = time;

			glfwWaitEvents();
		}
	}

	void init()
	{
		auto video = glfwGetVideoMode(glfwGetPrimaryMonitor());
		window_.setPosition(PointType{ (video->width - window_.size()(0)) * 0.5, (video->height - window_.size()(1)) * 0.5 });

		TextLoader::load(reinterpret_cast<const char *>(text_txt));
		// main window
		window_.setSize(SizeType{ 470.0f, 340.0f });
		// root panel
		root_panel_ = std::make_shared<decorator::Blurred<Widget>>(&window_);
		root_panel_->setPosition(PointType{ 10.0f, 10.0f });
		root_panel_->setSize(SizeType{ 450.0f, 320.0f });
		root_panel_->setBackgroundColor(nvgRGBA(0xff, 0xff, 0xff, 0xff));
		window_.setRootWidget(root_panel_);
		root_panel_->drop_signal.connect([this] (const std::string & path) {
			std::cout << path << std::endl;
			if (this->process(path)) {
				this->showState(1);
				
			}
			return true;
		});
        // title
        auto title_ = std::make_shared<StaticText>(root_panel_, TextLoader::get_text("title"));
        root_panel_->addChild(title_);
        title_->setPosition(PointType{ 235.0f, 20.0f });
		title_->setSize(SizeType{ 0.0f, 16.0f });
		title_->setColor(nvgRGBA(0x80, 0x80, 0x80, 0xff));
		title_->setFontSize(16);
		// close button
		auto close_button_ = std::make_shared<ImageButton>(root_panel_, close_png, close_png_size);
		root_panel_->addChild(close_button_);
		close_button_->setPosition(PointType{ 434.0f, 20.0f });
		close_button_->setSize(SizeType{ 16.0f, 16.0f });
		close_button_->setBackgroundColor(nvgRGBA(0xf4, 0x65, 0x59, 0xff));
		close_button_->setHoveredColor(nvgRGBA(0xf4, 0x65, 0x59, 0xcc));
		close_button_->setColor(nvgRGBA(0xff, 0xff, 0xff, 0xff));
		close_button_->setFontSize(12);

		close_button_->mouse_button_signal.connect([this](int button, int action, int mods)
		{
			if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
				this->close();
			}
			return false;
		});
        // info button
        auto info_button_ = std::make_shared<ImageButton>(root_panel_, info_png, info_png_size);
		root_panel_->addChild(info_button_);
		info_button_->setPosition(PointType{ 408.0f, 20.0f });
		info_button_->setSize(SizeType{ 16.0f, 16.0f });
		info_button_->setBackgroundColor(nvgRGBA(0xf4, 0x65, 0x59, 0xff));
		info_button_->setHoveredColor(nvgRGBA(0xf4, 0x65, 0x59, 0xcc));
		info_button_->setColor(nvgRGBA(0xff, 0xff, 0xff, 0xff));
		info_button_->setFontSize(12);

		info_button_->mouse_button_signal.connect([this](int button, int action, int mods)
		{
			if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
				if (this->about_ == nullptr) {
					this->about_ = std::make_shared<AboutDialog>();
					this->about_->init();
				} else {
					this->about_->show();
				}
			}
			return false;
		});

        // input box
        qr_text_input_ = std::make_shared<TextBox>(root_panel_, "");
		qr_text_input_->setHint(TextLoader::get_text("hint"));
		qr_text_input_->setHintColor(nvgRGBA(0x9c, 0x9c, 0x9c, 0xff));
        root_panel_->addChild(qr_text_input_);
		qr_text_input_->setPosition(PointType{ 35.0f, 270.0f });
		qr_text_input_->setSize(SizeType{ 400.0f, 40.0f });
		qr_text_input_->setColor(nvgRGBA(0x66, 0x66, 0x66, 0xff));
		qr_text_input_->setFontSize(18);
        qr_text_input_->setBackgroundColor(nvgRGBA(0xdf, 0xde, 0xe1, 0xff));

		qr_text_input_->char_signal.connect([this] (unsigned int) {
			this->showState(0);
			return false;
		});

		qr_text_input_->key_signal.connect([this](int, int, int, int) {
			this->showState(0);
			return false;
		});

        // state images
        changed_image_ = std::make_shared<moem::gui::Image>(root_panel_, changed_png, changed_png_size);
        root_panel_->addChild(changed_image_);
        changed_image_->setPosition(PointType { 156.5f, 80.0f });
        
        unchanged_image_ = std::make_shared<moem::gui::Image>(root_panel_, unchanged_png, unchanged_png_size);
        root_panel_->addChild(unchanged_image_);
        unchanged_image_->setPosition(PointType { 115.5f, 80.0f });
        showState(0);
	}
	
	void close()
	{
		window_.close();
	}

    void showState(int i)
    {
        if (i == 0) {
            changed_image_->setSize(SizeType { 0.0f, 0.0f });
            unchanged_image_->setSize(SizeType { 239.0f, 170.0f });
        } else {
            unchanged_image_->setSize(SizeType { 0.0f, 0.0f });
            changed_image_->setSize(SizeType { 157.0f, 170.0f });
        }
    }

	Eigen::Vector3i get_dominant_color(const std::string & filename)
	{
		int x, y, n;
		unsigned char *data = stbi_load(filename.c_str(), &x, &y, &n, 4);
		DominantColor dc(data, x, y, n);
		auto colors = dc.find_dominant_colors(1);
		stbi_image_free(data);
		std::cout << "Dominant Color: " << std::endl << colors[0] << std::endl;
		return colors[0];
	}

	bool process(const std::string & filename)
	{
#ifdef _WIN32
		if (qr_text_input_->value().length() == 0) {
			qr_text_input_->setHintColor(nvgRGBA(0xf4, 0x65, 0x59, 0xff));
			return false;
		}

		auto color = get_dominant_color(filename);
		int r = color(0) - 75;
		int g = color(1) - 75;
		int b = color(2) - 75;
		if (r < 0) r = 0;
		if (g < 0) g = 0;
		if (b < 0) b = 0;
		std::wstringstream os;
		
		static std::wstring_convert<std::codecvt_utf8<wchar_t> > cvt;
		STARTUPINFOW si;
		PROCESS_INFORMATION pi;
		auto ifilename = cvt.from_bytes(filename);
		auto ofilename = ifilename.substr(0, ifilename.find_last_of(L'.')) + L"_qr";
		os << L"dist\\CuteR.exe -o \"" << ofilename << L".png\" -C -r "
			<< r << L" " << g << L" " << b << L" \""
			<< ifilename << L"\" \"" << cvt.from_bytes(qr_text_input_->value()) << L"\"";

		LPWSTR command = new WCHAR[os.str().length() + 10]();
		wcscpy(command, os.str().c_str());
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		ZeroMemory(&pi, sizeof(pi));

		// Start the child process. 
		if (!CreateProcessW(NULL,   // No module name (use command line)
			command,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi)           // Pointer to PROCESS_INFORMATION structure
			)
		{
			printf("CreateProcess failed (%d).\n", GetLastError());
			return false;
		}

		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
#else
        auto ifilename = filename;
		auto ofilename = ifilename.substr(0, ifilename.find_last_of('.')) + "_qr";
		auto fname = "dist/CuteR -o \"" + ofilename + ".png\" -C -r 100 50 100 \"" + ifilename + "\" http://www.chinuno.com";
        std::cout << fname << std::endl;
        system(fname.c_str());
#endif
		return true;
	}

protected:
	Window window_;
	std::shared_ptr<moem::gui::Widget> root_panel_;
	std::shared_ptr<ImageButton> close_button_;
    std::shared_ptr<Image> changed_image_, unchanged_image_;
	std::shared_future<bool> convertion_finished_;
	std::shared_ptr<TextBox> qr_text_input_;
	std::shared_ptr<AboutDialog> about_;
};

int main(void)
{
	CuteRuiDialog dialog;
	dialog.init();
	dialog.main_loop();

	return 0;
}

#ifdef _WIN32

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return main();
}

#endif