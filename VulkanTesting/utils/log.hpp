#pragma once

#define UTIL_THROW(message) Log::Throw(__FILE__, __LINE__, message)
#define UTIL_LOG(message) Log::Log(__FILE__, __LINE__, message)

#include <iostream>

namespace Log
{
	inline void Throw(const char* fileName, const int line, const std::string& message) {
		auto exceptionMessage = std::string(fileName) + "[" + std::to_string(line) + "]: " + message + "\n";
		throw std::runtime_error(exceptionMessage.c_str());
	}

	inline void Log(const char* fileName, const int line, const std::string& message) {
		std::cout << fileName << " " << line << ": " << message << std::endl;
	}
}
