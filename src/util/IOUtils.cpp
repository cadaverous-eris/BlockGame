#include "IOUtils.h"

namespace eng {

	std::string readTextFile(const std::string& filePath) {
		std::string content;
		std::ifstream fileStream(filePath, std::ios::in);
		if (!fileStream.is_open()) {
			std::cerr << "Could not open file at " << filePath << std::endl;
			throw std::runtime_error("Could not open file at " + std::string(filePath));
		}
		std::string line {};
		while (!fileStream.eof()) {
			std::getline(fileStream, line);
			content.append(line + "\n");
		}
		return content;
	}
	void writeTextFile(const std::string& filePath, const std::string_view text) {
		std::ofstream fileStream(filePath, std::ios::out | std::ios::trunc);
		if (!fileStream.is_open()) {
			std::cerr << "Could not open file at " << filePath << std::endl;
			throw std::runtime_error("Could not open file at " + std::string(filePath));
		}
		fileStream << text;
	}

	std::vector<unsigned char> readFileBytes(const std::string& filePath) {
		std::ifstream fileStream(filePath, std::ios::in | std::ios::binary | std::ios::ate);
		if (!fileStream.is_open()) {
			std::cerr << "Could not open file at " << filePath << std::endl;
			throw std::runtime_error("Could not open file at " + std::string(filePath));
		}
		auto fileSize = fileStream.tellg();
		fileStream.seekg(0, std::ios::beg);
		std::vector<unsigned char> bytes;
		bytes.resize(fileSize);
		fileStream.read(reinterpret_cast<char*>(bytes.data()), fileSize);
		return bytes;
	}

}