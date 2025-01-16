#pragma once
#include <iostream>

#define UTIL_THROW(message) throw std::runtime_error(std::string(__FILE__) + "[" + std::to_string(__LINE__) + "]: " + message + "\n")
#define UTIL_LOG(message) std::cout << fileName << " " << line << ": " << message << std::endl