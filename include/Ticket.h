#pragma once
#include <ctime>
#include <ostream>
#include <string>
#include "Vehicle.h"

class Ticket {
    int id_;
    std::string plate_;
    std::string owner_;
    VehicleType type_;
    int slotId_;
    std::time_t entry_;
    std::time_t exit_ = 0;
    double rate_;
    int hours_ = 0;
    double fee_ = 0.0;

public:
    Ticket(int id, std::string plate, std::string owner, VehicleType type,
           int slotId, std::time_t entry, double rate);

    int id() const { return id_; }
    const std::string& plate() const { return plate_; }
    const std::string& owner() const { return owner_; }
    VehicleType type() const { return type_; }
    int slotId() const { return slotId_; }
    std::time_t entry() const { return entry_; }
    std::time_t exit() const { return exit_; }
    int hours() const { return hours_; }
    double fee() const { return fee_; }

    // Billing rule: rounded UP to the next hour, minimum 1 hour.
    void close(std::time_t exitTime);

    // Operator overloading: print a receipt.
    friend std::ostream& operator<<(std::ostream& os, const Ticket& t);
};
