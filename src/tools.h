#ifndef TOOLS_H
#define TOOLS_H
#include <sstream>
#include <string>
#include <vector>

namespace tools {
    /**
	 * \brief Splits a `std::string` around a given delimiter into a `std::vector<std::string>`.
	 *
	 * \param s Instance of `std::string` to split.
	 * \param delim `char` to split `s` around.
	 * \return A `std::vector` of `std::string` instances containing each sub-string after splits.
	 */
	std::vector<std::string> split(const std::string& s, char delim) {
		std::vector<std::string> elems;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) elems.push_back(item);
		return elems;
	}

	/**
	 * \brief Splits and parses a `std::string` around delimiters `delim` into a `std::vector<int>`.
	 * \param s `std::string` to split and parse.
	 * \param delim delimiter around which to split `s`.
	 * \return `std::vector<int>` of split and parsed values.
	 */
	std::vector<short> split_stos(const std::string& s, char delim) {
		std::vector<short> elems;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim))
			elems.push_back(static_cast<short>(std::stoi(item)));
		return elems;
	}

    /**
	 * \brief Removes all leading whitespace `char`s from a `std::string` instance.
	 *
	 * \param s Reference to `std::string` instance.
	 * \return Reference to `s` with all leading whitespaces removed.
	 */
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

	/**
	 * \brief Determines whether a `std::string` instance starts with a given `char`.
	 *
	 * \param s Instance of `std::string`.
	 * \param c `char` to check against.
	 * \return `true` if `s` starts with `c`, `false` otherwise.
	 */
	bool starts_with(const std::string& s, char c) {
		return *s.cbegin() == c;
	}
}

#endif // !TOOLS_H