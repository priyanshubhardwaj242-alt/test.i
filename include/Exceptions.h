#pragma once
#include <stdexcept>

// Custom exception used for every rule violation in the system
// (lot full, duplicate vehicle, invalid plate, corrupt data file...).
class ParkingException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
