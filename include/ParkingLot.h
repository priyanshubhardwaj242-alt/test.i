#pragma once
#include "ParkingSlot.h"
#include "Ticket.h"
#include <ctime>
#include <map>
#include <ostream>
#include <string>
#include <vector>

class ParkingLot {
    std::vector<ParkingSlot> slots_;
    std::map<std::string, Ticket> active_;  // plate -> open ticket
    std::vector<Ticket> history_;           // closed tickets

    ParkingSlot* findFreeSlot(VehicleType type);
    ParkingSlot* findSlot(const std::string& label);

public:
    ParkingLot(int bikeSlots, int carSlots, int truckSlots);

    // 'now' defaults to the system clock; tests pass fixed times.
    Ticket park(const Vehicle& vehicle, std::time_t now = std::time(nullptr));
    Ticket exitVehicle(const std::string& plate, std::time_t now = std::time(nullptr));
    const Ticket* find(const std::string& plate) const;

    int totalSlots(VehicleType type) const;
    int freeSlots(VehicleType type) const;
    double totalRevenue() const;
    std::size_t vehiclesServed() const { return history_.size(); }
    std::size_t vehiclesParked() const { return active_.size(); }

    void printStatus(std::ostream& os) const;
    void printHistory(std::ostream& os) const;

    void save(const std::string& path) const;
    void load(const std::string& path);  // call on a fresh lot
};
