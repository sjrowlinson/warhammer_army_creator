#include "tools.h"

namespace tools {

	std::vector<std::string> split(const std::string& s, char delim) {
		std::vector<std::string> elems;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) elems.push_back(item);
		return elems;
	}

	std::vector<short> split_stos(const std::string& s, char delim) {
		std::vector<short> elems;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim))
			elems.push_back(static_cast<short>(std::stoi(item)));
		return elems;
	}

	std::string& remove_leading_whitespaces(std::string& s) {
		auto it = s.begin();
		// loop until past-the-end iterator of s
		while (it != s.end()) {
            // if character is a space, erase it and set it to
			// next valid std::string::iterator in s
			if (std::isspace(*it)) it = s.erase(it);
			// if character is not a space, finished so break
			else break;
		}
		return s;
	}

	bool starts_with(const std::string& s, char c) {
		return *s.cbegin() == c;
	}
}