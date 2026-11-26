#include "ParkingSlot.h"
#include "Exceptions.h"

ParkingSlot::ParkingSlot(std::string label, VehicleType type)
    : label_(std::move(label)), type_(type) {}

void ParkingSlot::occupy(const std::string& plate) {
    if (!isFree()) throw ParkingException("Slot " + label_ + " is already occupied.");
    plate_ = plate;
}

void ParkingSlot::release() { plate_.clear(); }
