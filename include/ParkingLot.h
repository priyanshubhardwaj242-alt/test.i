#pragma once
#include <ctime>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include "ParkingSlot.h"
#include "Ticket.h"

class ParkingLot {
    std::vector<ParkingSlot> slots_;
    std::map<std::string, Ticket> active_;  // plate -> ticket
    int nextTicketId_ = 1;
    double revenue_ = 0.0;
    std::string dataDir_;

    ParkingSlot* findFreeSlot(VehicleType t);
    ParkingSlot* slotById(int id);

public:
    ParkingLot(int bikeSlots, int carSlots, int truckSlots,
               std::string dataDir = "data");

    Ticket park(VehicleType type, const std::string& plate,
                const std::string& owner, std::time_t at = std::time(nullptr));
    Ticket unpark(const std::string& plate, std::time_t at = std::time(nullptr));

    const Ticket* find(const std::string& plate) const;
    int totalSlots(VehicleType t) const;
    int freeSlots(VehicleType t) const;
    double revenue() const { return revenue_; }
    std::size_t activeCount() const { return active_.size(); }

    void printStatus(std::ostream& os) const;
    void printActive(std::ostream& os) const;
    void printHistory(std::ostream& os) const;

    // File handling (persistence between runs)
    void save() const;
    bool load();  // false if no saved state exists
};
