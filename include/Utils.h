#pragma once
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

struct InputClosed {};  // thrown when stdin ends (Ctrl+D / Ctrl+Z)

inline std::string formatTime(std::time_t t) {
    char buf[32];
    std::tm* tm = std::localtime(&t);
    std::strftime(buf, sizeof buf, "%Y-%m-%d %H:%M", tm);
    return buf;
}

inline std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) throw InputClosed{};
    return line;
}

// Function template: keeps asking until the user types a valid T.
template <typename T>
T readValue(const std::string& prompt) {
    for (;;) {
        std::istringstream iss(readLine(prompt));
        T value;
        if (iss >> value && (iss >> std::ws).eof()) return value;
        std::cout << "Invalid input, please try again.\n";
    }
}
