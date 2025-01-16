#pragma once
#include <iostream>

#define UTIL_THROW(message) throw std::runtime_error(std::string(__FILE__) + "[" + std::to_string(__LINE__) + "]: " + message + "\n")
#define UTIL_LOG(message) system("Color 0A"); std::cout << __FILE__ << " " << __LINE__ << ": " << std::string("") + message << std::endl
#define UTIL_ERR(message) std::cerr << __FILE__ << " " << __LINE__ << ": " << std::string("") + message << std::endl
#define UTIL_WARN(message) system("Color 06"); std::cout << __FILE__ << " " << __LINE__ << ": " << std::string("") + message << std::endl
