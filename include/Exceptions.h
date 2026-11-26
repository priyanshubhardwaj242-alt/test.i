#pragma once
#include <stdexcept>
#include <string>

// Custom exception hierarchy: every parking error IS-A ParkingException,
// which IS-A std::runtime_error.
class ParkingException : public std::runtime_error {
public:
    explicit ParkingException(const std::string& msg) : std::runtime_error(msg) {}
};

class LotFullException : public ParkingException {
public:
    explicit LotFullException(const std::string& type)
        : ParkingException("No free slot available for vehicle type: " + type) {}
};

class DuplicateVehicleException : public ParkingException {
public:
    explicit DuplicateVehicleException(const std::string& plate)
        : ParkingException("Vehicle " + plate + " is already parked.") {}
};

class VehicleNotFoundException : public ParkingException {
public:
    explicit VehicleNotFoundException(const std::string& plate)
        : ParkingException("Vehicle " + plate + " is not parked here.") {}
};

class InvalidInputException : public ParkingException {
public:
    explicit InvalidInputException(const std::string& msg) : ParkingException(msg) {}
};
