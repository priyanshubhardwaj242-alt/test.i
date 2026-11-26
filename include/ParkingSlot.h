#pragma once
#include <memory>
#include "Vehicle.h"

// Encapsulation: state is private, accessed via methods.
class ParkingSlot {
    int id_;
    VehicleType type_;
    std::shared_ptr<Vehicle> vehicle_;

public:
    ParkingSlot(int id, VehicleType type) : id_(id), type_(type) {}

    int id() const { return id_; }
    VehicleType type() const { return type_; }
    bool isFree() const { return vehicle_ == nullptr; }
    const std::shared_ptr<Vehicle>& vehicle() const { return vehicle_; }

    void park(std::shared_ptr<Vehicle> v) { vehicle_ = std::move(v); }
    void vacate() { vehicle_.reset(); }
};
