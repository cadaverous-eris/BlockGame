#include "IOUtils.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>

#include <zstr.hpp>

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

	std::vector<unsigned char> readBinaryFile(const std::string& filePath) {
		std::ifstream fileStream(filePath, std::ios::in | std::ios::binary | std::ios::ate);
		if (!fileStream.is_open()) {
			std::cerr << "Could not open file at " << filePath << std::endl;
			throw std::runtime_error("Could not open file at " + std::string(filePath));
		}
		const auto fileSize = fileStream.tellg();
		fileStream.seekg(0, std::ios::beg);
		std::vector<unsigned char> bytes;
		bytes.resize(fileSize);
		fileStream.read(reinterpret_cast<char*>(bytes.data()), fileSize);
		return bytes;
	}
	void writeBinaryFile(const std::string& filePath, std::span<const unsigned char> bytes) {
		std::ofstream fileStream(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
		if (!fileStream.is_open()) {
			std::cerr << "Could not open file at " << filePath << std::endl;
			throw std::runtime_error("Could not open file at " + std::string(filePath));
		}
		// TODO: handle byte spans with sizes too large to be represented by std::streamsize (std::ptrdiff_t)?
		fileStream.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size_bytes()));
	}

	std::vector<unsigned char> readCompressedBinaryFile(const std::string& filePath) {
		std::ifstream fileStream(filePath, std::ios::in | std::ios::binary | std::ios::ate);
		if (!fileStream.is_open()) {
			std::cerr << "Could not open file at " << filePath << std::endl;
			throw std::runtime_error("Could not open file at " + std::string(filePath));
		}
		const auto fileSize = fileStream.tellg();
		std::cout << "Compressed size of '" << filePath << "': " << fileSize << '\n';
		fileStream.seekg(0, std::ios::beg);
		zstr::istream zFileStream(fileStream);
		const size_t bufSize = 1 << 16;
		auto buf = std::make_unique<char[]>(bufSize);
		std::vector<unsigned char> bytes;
		while (true) {
			zFileStream.read(buf.get(), bufSize);
        	size_t cnt = static_cast<size_t>(zFileStream.gcount());
			if (cnt == 0) {
				break;
			} else {
				const size_t prevBytesSize = bytes.size();
				bytes.resize(prevBytesSize + cnt);
				std::memcpy(bytes.data() + prevBytesSize, buf.get(), cnt);
			}
		};
		return bytes;
	}
	void writeCompressedBinaryFile(const std::string& filePath, std::span<const unsigned char> bytes) {
		std::ofstream fileStream(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
		if (!fileStream.is_open()) {
			std::cerr << "Could not open file at " << filePath << std::endl;
			throw std::runtime_error("Could not open file at " + std::string(filePath));
		}
		zstr::ostream zFileStream(fileStream);
		// TODO: handle byte spans with sizes too large to be represented by std::streamsize (std::ptrdiff_t)?
		zFileStream.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size_bytes()));
	}

}