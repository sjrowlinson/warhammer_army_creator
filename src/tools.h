#ifndef TOOLS_H
#define TOOLS_H
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace tools {
	std::vector<std::string> split(const std::string& s, char delim);
	std::vector<short> split_stos(const std::string& s, char delim);

	std::string& remove_leading_whitespaces(std::string& s);
	std::string remove_leading_whitespaces(const std::string& s);

    std::string& remove_trailing_whitespaces(std::string& s);

	bool starts_with(const std::string& s, char c);
	bool starts_with(const std::string& s, std::string match);
}

#endif // !TOOLS_H
