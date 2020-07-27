#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace eng {

	std::string readTextFile(const std::string& filePath);
	void writeTextFile(const std::string& filePath, const std::string_view text);

	std::vector<unsigned char> readFileBytes(const std::string& filePath);

	// TODO: functions for reading and writing image files

	// TODO: functions for reading and writing jonk files

}
