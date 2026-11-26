#include "Vehicle.h"
#include "Exceptions.h"
#include <cctype>

std::string toString(VehicleType type) {
    switch (type) {
        case VehicleType::Bike:  return "Bike";
        case VehicleType::Car:   return "Car";
        case VehicleType::Truck: return "Truck";
    }
    return "Unknown";
}

VehicleType vehicleTypeFromString(const std::string& text) {
    if (text == "Bike")  return VehicleType::Bike;
    if (text == "Car")   return VehicleType::Car;
    if (text == "Truck") return VehicleType::Truck;
    throw ParkingException("Unknown vehicle type: " + text);
}

std::string Vehicle::normalizePlate(const std::string& raw) {
    std::string out;
    for (char c : raw) {
        unsigned char u = static_cast<unsigned char>(c);
        if (std::isalnum(u))            out += static_cast<char>(std::toupper(u));
        else if (c == ' ' || c == '-')  continue;
        else throw ParkingException("Plate may contain only letters, digits, spaces and dashes.");
    }
    if (out.size() < 4 || out.size() > 12)
        throw ParkingException("Plate must have 4 to 12 letters/digits.");
    return out;
}

Vehicle::Vehicle(const std::string& plate) : plate_(normalizePlate(plate)) {}

std::string Vehicle::describe() const { return toString(type()) + " " + plate_; }

std::unique_ptr<Vehicle> makeVehicle(VehicleType type, const std::string& plate) {
    switch (type) {
        case VehicleType::Bike:  return std::make_unique<Bike>(plate);
        case VehicleType::Car:   return std::make_unique<Car>(plate);
        case VehicleType::Truck: return std::make_unique<Truck>(plate);
    }
    throw ParkingException("Unsupported vehicle type.");
}
