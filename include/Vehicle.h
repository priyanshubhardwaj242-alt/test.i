#pragma once
#include <memory>
#include <string>

enum class VehicleType { Bike, Car, Truck };

std::string toString(VehicleType type);
VehicleType vehicleTypeFromString(const std::string& text);

// Abstract base class: every concrete vehicle must define its type and rate.
class Vehicle {
protected:
    std::string plate_;  // encapsulated, normalised registration number

public:
    explicit Vehicle(const std::string& plate);
    virtual ~Vehicle() = default;

    const std::string& plate() const { return plate_; }
    virtual VehicleType type() const = 0;       // pure virtual
    virtual double ratePerHour() const = 0;     // pure virtual
    virtual std::string describe() const;

    // "ka 01-ab 1234" -> "KA01AB1234"; throws ParkingException if invalid.
    static std::string normalizePlate(const std::string& raw);
};

class Bike : public Vehicle {
public:
    using Vehicle::Vehicle;
    VehicleType type() const override { return VehicleType::Bike; }
    double ratePerHour() const override { return 10.0; }
};

class Car : public Vehicle {
public:
    using Vehicle::Vehicle;
    VehicleType type() const override { return VehicleType::Car; }
    double ratePerHour() const override { return 20.0; }
};

class Truck : public Vehicle {
public:
    using Vehicle::Vehicle;
    VehicleType type() const override { return VehicleType::Truck; }
    double ratePerHour() const override { return 40.0; }
};

// Factory function: returns the right subclass behind a base-class pointer.
std::unique_ptr<Vehicle> makeVehicle(VehicleType type, const std::string& plate);
