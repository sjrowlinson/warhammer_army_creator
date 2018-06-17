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
	std::vector<std::string> split(const std::string& s, char delim);

	/**
	 * \brief Splits and parses a `std::string` around delimiters `delim` into a `std::vector<int>`.
	 * \param s `std::string` to split and parse.
	 * \param delim delimiter around which to split `s`.
	 * \return `std::vector<int>` of split and parsed values.
	 */
	std::vector<short> split_stos(const std::string& s, char delim);

    /**
	 * \brief Removes all leading whitespace `char`s from a `std::string` instance.
	 *
	 * \param s Reference to `std::string` instance.
	 * \return Reference to `s` with all leading whitespaces removed.
	 */
	std::string& remove_leading_whitespaces(std::string& s);
	std::string remove_leading_whitespaces(const std::string& s);

	/**
	 * \brief Determines whether a `std::string` instance starts with a given `char`.
	 *
	 * \param s Instance of `std::string`.
	 * \param c `char` to check against.
	 * \return `true` if `s` starts with `c`, `false` otherwise.
	 */
	bool starts_with(const std::string& s, char c);
	bool starts_with(const std::string& s, std::string match);
}

#endif // !TOOLS_H