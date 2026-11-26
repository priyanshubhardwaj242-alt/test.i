#pragma once
#include <ctime>
#include <string>
#include <vector>

namespace util {
std::string trim(const std::string& s);
std::string toUpper(std::string s);
std::vector<std::string> split(const std::string& s, char delim);
std::string formatTime(std::time_t t);
// Normalises (trim + upper-case) and validates; throws InvalidInputException.
std::string normalizePlate(const std::string& plate);
std::string validateOwner(const std::string& owner);
}  // namespace util
