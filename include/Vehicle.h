#pragma once
#include <memory>
#include <string>

enum class VehicleType { BIKE = 0, CAR = 1, TRUCK = 2 };

std::string toString(VehicleType t);

// Abstract base class: demonstrates abstraction + polymorphism.
class Vehicle {
protected:
    std::string plate_;
    std::string owner_;

public:
    Vehicle(std::string plate, std::string owner);
    virtual ~Vehicle() = default;

    const std::string& plate() const { return plate_; }
    const std::string& owner() const { return owner_; }

    virtual VehicleType type() const = 0;      // pure virtual
    virtual double ratePerHour() const = 0;    // pure virtual
    virtual std::string describe() const;      // overridable
};

class Bike : public Vehicle {
public:
    using Vehicle::Vehicle;
    VehicleType type() const override { return VehicleType::BIKE; }
    double ratePerHour() const override { return 10.0; }
};

class Car : public Vehicle {
public:
    using Vehicle::Vehicle;
    VehicleType type() const override { return VehicleType::CAR; }
    double ratePerHour() const override { return 20.0; }
};

class Truck : public Vehicle {
public:
    using Vehicle::Vehicle;
    VehicleType type() const override { return VehicleType::TRUCK; }
    double ratePerHour() const override { return 50.0; }
    std::string describe() const override;
};

// Factory function
std::shared_ptr<Vehicle> makeVehicle(VehicleType t, const std::string& plate,
                                     const std::string& owner);
