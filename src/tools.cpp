#include "tools.h"

namespace tools {

	std::vector<std::string> split(const std::string& s, char delim) {
		std::vector<std::string> elems;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) elems.push_back(item);
		for (auto& _s : elems) tools::remove_leading_whitespaces(_s);
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
		while (it != s.end() && std::isspace(*it)) it = s.erase(it);
		return s;
	}

	std::string remove_leading_whitespaces(const std::string& s) {
		std::string tmp = s;
		auto it = tmp.begin();
		while (it != tmp.end() && std::isspace(*it)) it = tmp.erase(it);
		return tmp;
	}

	bool starts_with(const std::string& s, char c) {
		if (s.empty()) return false;
		return *s.cbegin() == c;
	}
	bool starts_with(const std::string& s, std::string match) {
		if (s.empty() && match.empty()) return true;
		if (s.empty()) return false;
		for (std::size_t i = 0U; i < match.size(); ++i) {
			if (s[i] != match[i]) return false;
		}
		return true;
	}

}