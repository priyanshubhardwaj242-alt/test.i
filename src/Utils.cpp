#include "Utils.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include "Exceptions.h"

namespace util {

std::string trim(const std::string& s) {
    std::size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

std::string toUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return s;
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> out;
    std::string item;
    std::istringstream ss(s);
    while (std::getline(ss, item, delim)) out.push_back(item);
    return out;
}

std::string formatTime(std::time_t t) {
    std::tm tmv{};
#ifdef _WIN32
    localtime_s(&tmv, &t);
#else
    localtime_r(&t, &tmv);
#endif
    std::ostringstream os;
    os << std::put_time(&tmv, "%Y-%m-%d %H:%M");
    return os.str();
}

std::string normalizePlate(const std::string& plate) {
    std::string p = toUpper(trim(plate));
    if (p.size() < 4 || p.size() > 12)
        throw InvalidInputException("Plate must be 4-12 characters long.");
    for (unsigned char c : p) {
        if (!(std::isalnum(c) || c == '-' || c == ' '))
            throw InvalidInputException(
                "Plate may only contain letters, digits, '-' and spaces.");
    }
    return p;
}

std::string validateOwner(const std::string& owner) {
    std::string o = trim(owner);
    if (o.empty()) throw InvalidInputException("Owner name cannot be empty.");
    if (o.find('|') != std::string::npos)
        throw InvalidInputException("Owner name cannot contain '|'.");
    return o;
}

}  // namespace util
