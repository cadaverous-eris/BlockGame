#include "text_utils.h"

namespace eng {

	std::string toLowerCase(std::string str) {
		for (auto& c : str) {
			if ((c >= 'A') && (c <= 'Z')) c += ('a' - 'A');
		}
		return str;
	}
	std::string toLowerCase(const std::string_view str) {
		std::string result(str);
		for (auto& c : result) {
			if ((c >= 'A') && (c <= 'Z')) c += ('a' - 'A');
		}
		return result;
	}
	std::string toUpperCase(std::string str) {
		for (auto& c : str) {
			if ((c >= 'a') && (c <= 'z')) c += ('A' - 'a');
		}
		return str;
	}
	std::string toUpperCase(const std::string_view str) {
		std::string result(str);
		for (auto& c : result) {
			if ((c >= 'a') && (c <= 'z')) c += ('A' - 'a');
		}
		return result;
	}

}