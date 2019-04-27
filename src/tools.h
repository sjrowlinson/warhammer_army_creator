#ifndef TOOLS_H
#define TOOLS_H
#include "base_unit.h"
#include "magic_item.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace tools {
	std::vector<std::string> split(const std::string& s, char delim);
	std::vector<short> split_stos(const std::string& s, char delim);
    std::string points_str(double value);
    std::string points_str(double value, BaseUnitType but);

	std::string& remove_leading_whitespaces(std::string& s);
	std::string remove_leading_whitespaces(const std::string& s);

    std::string& remove_trailing_whitespaces(std::string& s);
    std::string remove_trailing_whitespaces(const std::string& s);

    std::string& trim(std::string& s);
    std::string trim(const std::string& s);

	bool starts_with(const std::string& s, char c);
    bool ends_with(const std::string& s, char c);
	bool starts_with(const std::string& s, std::string match);

    std::unordered_map<std::string, item> magic_items_of(
        const std::unordered_map<std::string, item>& items, ItemType item_type
    );
    std::vector<std::pair<std::string, item>> magic_items_vec_of(
            const std::unordered_map<std::string, item> &items,
            ItemType item_type
    );

    template<template<class...> class Ty, class Inner = std::string>
    constexpr Ty<std::pair<Inner, Inner>> zip_args_to_names_values(const Ty<Inner>& container, char delim=':') {
        Ty<std::pair<Inner, Inner>> zipped(container.size());
        std::generate(std::begin(zipped), std::end(zipped), [delim, it=std::begin(container)]() mutable {
            auto s = split(*(it++), delim);
            return std::make_pair(s[0], s.size() > 1 ? s[1] : Inner());
        });
        return zipped;
    }

    bool in_names_values(const std::vector<std::pair<std::string, std::string>>& names_values, std::string s);

    template<class TyF, class TyS>
    constexpr std::vector<std::pair<TyF, TyS>> umap_to_vector(const std::unordered_map<TyF, TyS>& umap) {
        std::vector<std::pair<TyF, TyS>> vec;
        for (const auto& p : umap) vec.push_back(p);
        return vec;
    }

    template<class InputIt, class UnaryPredicate>
    constexpr std::vector<InputIt> find_all_if(InputIt first, InputIt last, UnaryPredicate p) {
        std::vector<InputIt> found;
        for (; first != last; ++first) {
            if (p(*first)) found.push_back(first);
        }
        return found;
    }

}

#endif // !TOOLS_H
