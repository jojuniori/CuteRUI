#include "TextLoader.hpp"

#include <iostream>
#include <sstream>

TextLoader * TextLoader::loader_ = nullptr;

std::string TextLoader::get_text(const std::string &name)
{
	return loader_->map_[name];
}

void TextLoader::load(const char * content)
{
	loader_ = new TextLoader(content);
}

TextLoader::TextLoader(const char * content)
{
	std::istringstream ifs(content);
	std::string key, value;
	while (!ifs.eof()) {
		std::getline(ifs, key, '=');
		std::getline(ifs, value);
		map_[key] = value;
	}

}

TextLoader::~TextLoader()
{
}