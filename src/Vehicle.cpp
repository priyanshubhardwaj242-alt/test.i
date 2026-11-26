#include "Vehicle.h"
#include <sstream>
#include "Exceptions.h"

std::string toString(VehicleType t) {
    switch (t) {
        case VehicleType::BIKE:  return "Bike";
        case VehicleType::CAR:   return "Car";
        case VehicleType::TRUCK: return "Truck";
    }
    return "Unknown";
}

Vehicle::Vehicle(std::string plate, std::string owner)
    : plate_(std::move(plate)), owner_(std::move(owner)) {}

std::string Vehicle::describe() const {
    std::ostringstream os;
    os << toString(type()) << " [" << plate_ << "] owner: " << owner_
       << " (Rs. " << ratePerHour() << "/hr)";
    return os.str();
}

std::string Truck::describe() const {
    return Vehicle::describe() + " - heavy vehicle";
}

std::shared_ptr<Vehicle> makeVehicle(VehicleType t, const std::string& plate,
                                     const std::string& owner) {
    switch (t) {
        case VehicleType::BIKE:  return std::make_shared<Bike>(plate, owner);
        case VehicleType::CAR:   return std::make_shared<Car>(plate, owner);
        case VehicleType::TRUCK: return std::make_shared<Truck>(plate, owner);
    }
    throw InvalidInputException("Unknown vehicle type.");
}
