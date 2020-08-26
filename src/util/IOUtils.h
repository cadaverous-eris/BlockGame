#pragma once

#include <string>
#include <vector>
#include <span>

namespace eng {

	std::string readTextFile(const std::string& filePath);
	void writeTextFile(const std::string& filePath, const std::string_view text);

	std::vector<unsigned char> readBinaryFile(const std::string& filePath);
	void writeBinaryFile(const std::string& filePath, std::span<const unsigned char> bytes);

	std::vector<unsigned char> readCompressedBinaryFile(const std::string& filePath);
	void writeCompressedBinaryFile(const std::string& filePath, std::span<const unsigned char> bytes);


	// TODO: functions for reading and writing image files

	// TODO: functions for reading and writing jonk files?

}
