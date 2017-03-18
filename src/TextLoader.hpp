#ifndef MW_TEXT_LOADER_HPP
#define MW_TEXT_LOADER_HPP

#include <vector>
#include <string>
#include <map>

class TextLoader
{
public:
	static std::string get_text(const std::string &name);
	static void load(const char * content);
	~TextLoader();
protected:
	TextLoader(const char * content);
	static TextLoader * loader_;
	std::map<std::string, std::string> map_;
};

#endif