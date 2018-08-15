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

    std::string points_str(double value) {
        auto tmp = tools::split(std::to_string(value), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        return (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
    }

    std::string points_str(double value, BaseUnitType but) {
        auto pts_str = points_str(value);
        return pts_str + " pts" + ((but == BaseUnitType::NORMAL) ? "/model" : "");
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

    std::string& remove_trailing_whitespaces(std::string& s) {
        auto it = s.rbegin();
        while (it != s.rend() && std::isspace(*it)) {
            ++it;
            it = std::reverse_iterator<std::string::iterator>(s.erase(it.base()));
        }
        return s;
    }

    std::string remove_trailing_whitespaces(const std::string& s) {
        std::string tmp = s;
        auto it = tmp.rbegin();
        while (it != tmp.rend() && std::isspace(*it)) {
            ++it;
            it = std::reverse_iterator<std::string::iterator>(tmp.erase(it.base()));
        }
        return tmp;
    }

    std::string& trim(std::string& s) {
        remove_leading_whitespaces(s);
        return remove_trailing_whitespaces(s);
    }

    std::string trim(const std::string& s) {
        auto t1 = remove_leading_whitespaces(s);
        return remove_trailing_whitespaces(t1);
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

    std::vector<std::string> parse_item_bs(std::string s) {
        std::vector<std::string> brace_start = tools::split(s, '{');
        std::vector<std::string> brace_end = tools::split(brace_start[1], '}');
        std::vector<std::string> square_start = tools::split(brace_end[1], '[');
        std::vector<std::string> square_end = tools::split(square_start[1], ']');
        std::vector<std::string> result;
        result.push_back(brace_start[0]);
        result.push_back(brace_end[0]);
        result.push_back(square_end[0]);
        return result;
    }

    std::vector<std::string> parse_item_bsp(std::string s) {
        std::vector<std::string> brace_start = tools::split(s, '{');
        std::vector<std::string> brace_end = tools::split(brace_start[1], '}');
        std::vector<std::string> square_start = tools::split(brace_end[1], '[');
        std::vector<std::string> square_end = tools::split(square_start[1], ']');
        std::vector<std::string> paren_start = tools::split(square_end[1], '(');
        std::vector<std::string> paren_end = tools::split(paren_start[1], ')');
        std::vector<std::string> result;
        result.push_back(brace_start[0]);
        result.push_back(brace_end[0]);
        result.push_back(square_end[0]);
        result.push_back(paren_end[0]);
        return result;
    }

    std::unordered_map<std::string, item> magic_items_of(
        const std::unordered_map<std::string, item> &items,
        ItemType item_type
    ) {
        std::unordered_map<std::string, item> map;
        auto found = tools::find_all_if(
            items.begin(),
            items.end(),
            [item_type](const auto& x) { return x.second.item_type == item_type; }
        );
        for (auto it : found) map[it->first] = it->second;
        return map;
    }

    std::vector<std::pair<std::string, item>> magic_items_vec_of(
            const std::unordered_map<std::string, item> &items,
            ItemType item_type
    ) {
        std::vector<std::pair<std::string, item>> vec;
        auto found = tools::find_all_if(
            items.begin(),
            items.end(),
            [item_type](const auto& x) { return x.second.item_type == item_type; }
        );
        for (auto it : found) vec.push_back(*it);
        return vec;
    }

}
