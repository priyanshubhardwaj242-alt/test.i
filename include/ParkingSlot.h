#pragma once
#include "Vehicle.h"
#include <string>

class ParkingSlot {
    std::string label_;   // e.g. "C3"
    VehicleType type_;    // which vehicles may use this slot
    std::string plate_;   // empty when free

public:
    ParkingSlot(std::string label, VehicleType type);

    const std::string& label() const { return label_; }
    VehicleType type() const { return type_; }
    bool isFree() const { return plate_.empty(); }
    const std::string& plate() const { return plate_; }

    void occupy(const std::string& plate);  // throws if already occupied
    void release();
};
