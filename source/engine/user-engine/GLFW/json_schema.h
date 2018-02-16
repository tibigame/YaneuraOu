#ifndef _JSON_SCHEMA_H_
#define _JSON_SCHEMA_H_
#include "./lib/picojson.h"
#include <fstream>

constexpr char* json_file = "resource/setting.json";
constexpr char* root_path1 = "resource/";
constexpr char* root_path2 = "C:/Windows/Fonts";

struct FontSetting {
	std::string name;
	std::string path;
};

FontSetting read_file() {
	std::ifstream ifs(json_file);
	picojson::value v;
	ifs >> v;
	std::string err = picojson::get_last_error();
	if (!err.empty()) {
		exit(EXIT_FAILURE);
	}
	FontSetting result;
	if (v.is<picojson::array>()) {
		picojson::array& a = v.get<picojson::array>();
		for (picojson::array::iterator i = a.begin(); i != a.end(); ++i) {
			picojson::object& o = (*i).get<picojson::object>();
			result.name = o["name"].get<std::string>();
			result.path = o["path"].get<std::string>();
			std::ifstream infile1(root_path1 + result.path);
			if (infile1.good()) {
				result.path = root_path1 + result.path;
				return result;
			}
			std::ifstream infile2(root_path2 + result.path);
			if (infile2.good()) {
				result.path = root_path2 + result.path;
				return result;
			}
		}
	}
	return result;
}

#endif _JSON_SCHEMA_H_
