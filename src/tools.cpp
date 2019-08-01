#include "tools.h"

namespace tools {

	std::vector<std::string> split(const std::string& s, char delim) {
		std::vector<std::string> elems;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) elems.push_back(item);
        for (auto& _s : elems) tools::trim(_s);
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
        for (auto& s : tmp) tools::trim(s);
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

    std::string to_upper(std::string s) {
        std::string s_cap = s;
        std::transform(std::begin(s), std::end(s), std::begin(s_cap), [](unsigned char c) {
            return std::toupper(c);
        });
        return s_cap;
    }

	bool starts_with(const std::string& s, char c) {
		if (s.empty()) return false;
		return *s.cbegin() == c;
    }
    bool ends_with(const std::string& s, char c) {
        if (s.empty()) return false;
        return *(--s.cend()) == c;
    }
	bool starts_with(const std::string& s, std::string match) {
        return std::equal(
            std::begin(s), std::begin(s) + static_cast<std::string::difference_type>(match.size()),
            std::begin(match));
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

    bool in_names_values(const std::vector<std::pair<std::string, std::string>>& names_values, std::string s) {
        return std::count_if(
            std::begin(names_values),
            std::end(names_values),
            [&s](const auto& nv) { return nv.first == s; }
        );
    }


    void serialise_weapon(
        const std::unordered_map<
            WeaponType,
            std::tuple<ItemCategory, std::string, double>
        >& weapons,
        WeaponType wt,
        std::string& stream,
        std::string pre
    ) {
        if (!weapons.count(wt)) return;

        auto weapon = weapons.at(wt);
        ItemCategory category = std::get<0>(weapon);
        std::string name = std::get<1>(weapon);
        stream += pre + to_upper(enum_convert::WEAPON_TYPE_TO_STRING.at(wt)) + "_WEAPON = name: " + name +
                  ", category: " + enum_convert::ITEM_CLASS_TO_STRING.at(category);
    }

    void serialise_armour(
        const std::unordered_map<
            ArmourType,
            std::tuple<ItemCategory, std::string, double>
        >& armours,
        ArmourType at,
        std::string& stream,
        std::string pre
    ) {
        if (!armours.count(at)) return;

        auto armour = armours.at(at);
        ItemCategory category = std::get<0>(armour);
        std::string name = std::get<1>(armour);
        stream += pre + to_upper(enum_convert::ARMOUR_TYPE_TO_STRING.at(at)) + "_ARMOUR = name: " + name +
                  ", category: " + enum_convert::ITEM_CLASS_TO_STRING.at(category);
    }

    void serialise_magic_item(
        const std::pair<
            std::string,
            std::pair<ItemCategory, double>
        >& mitem,
        std::string type,
        std::string& stream,
        std::string pre
    ) {
        if (mitem.first.empty()) return;

        ItemCategory category = mitem.second.first;
        std::string name = mitem.first;
        stream += pre + to_upper(type) + " = name: " + name + ", category: "
                  + enum_convert::ITEM_CLASS_TO_STRING.at(category);
    }

    void serialise_oco_extra(
        const std::pair<
            std::string,
            std::pair<bool, double>
        >& oco,
        std::string& stream,
        std::string pre
    ) {
        if (oco.first.empty()) return;

        std::string name = oco.first;
        stream += pre + "OCO_EXTRA = " + name;
    }

    void serialise_mc_extras(
        const std::unordered_map<
            std::string,
            std::pair<bool, double>
        >& mc,
        std::string& stream,
        std::string pre
    ) {
        if (mc.empty()) return;

        stream += pre + "MC_EXTRAS = ";
        for (const auto& item_details : mc) {
            stream += item_details.first + "; ";
        }
    }

    void serialise_mount(
        const std::tuple<
            std::string,
            double,
            std::pair<std::string, double>,
            std::unordered_map<std::string, double>
        >& mnt,
        std::string& stream,
        std::string pre
    ) {
        if (std::get<0>(mnt).empty()) return;

        stream += pre + "MOUNT = " + std::get<0>(mnt);
        // serialise OCO option of mount
        if (!std::get<2>(mnt).first.empty()) {
            stream += pre + "MOUNT_OCO_OPTION = " + std::get<2>(mnt).first;
        }
        if (!std::get<3>(mnt).empty()) {
            stream += pre + "MOUNT_MC_OPTIONS = ";
            for (const auto& option_details : std::get<3>(mnt)) {
                stream += option_details.first + "; ";
            }
        }
    }

    void serialise_command_group(
        const std::unordered_map<
            CommandGroup, std::pair<std::string, double>
        >& command,
        std::string& stream,
        std::string pre
    ) {
        if (command.empty()) return;

        stream += pre + "COMMAND = ";
        for (const auto& comm_member : command) {
            stream += enum_convert::COMMAND_TO_STRING.at(comm_member.first) + "; ";
        }
    }

}
