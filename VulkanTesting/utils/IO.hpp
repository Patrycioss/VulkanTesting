#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "log.hpp"

namespace utils::io
{
	inline static std::string path = std::string(BINARY_DIR) + "/resources/"; 
	
	inline std::vector<char> readToBytes(const std::string& fileName) {
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			UTIL_THROW("Failed to open file " + fileName);
		}

		const size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}
}
