#pragma once
#include "Vehicle.h"
#include <ctime>
#include <ostream>
#include <string>

// One parking session: created on entry, closed on exit (fee calculated).
class Ticket {
    static int nextId_;  // static member shared by all tickets

    int id_;
    std::string plate_;
    VehicleType type_;
    std::string slot_;
    double rate_;
    std::time_t entry_;
    std::time_t exit_;   // 0 while the vehicle is still parked
    double fee_;

    Ticket(int id, std::string plate, VehicleType type, std::string slot,
           double rate, std::time_t entry, std::time_t exit, double fee);

public:
    Ticket(const std::string& plate, VehicleType type, const std::string& slot,
           double rate, std::time_t entry);

    static void setNextId(int id) { nextId_ = id; }
    static int peekNextId() { return nextId_; }

    int id() const { return id_; }
    const std::string& plate() const { return plate_; }
    VehicleType type() const { return type_; }
    const std::string& slot() const { return slot_; }
    std::time_t entryTime() const { return entry_; }
    std::time_t exitTime() const { return exit_; }
    double fee() const { return fee_; }
    bool isClosed() const { return exit_ != 0; }

    // Billable hours: parking time rounded UP, minimum 1 hour.
    long billableHours(std::time_t until) const;
    void close(std::time_t exitTime);

    // CSV persistence: id,plate,type,slot,rate,entry,exit,fee
    std::string toRecord() const;
    static Ticket fromRecord(const std::string& line);

    friend std::ostream& operator<<(std::ostream& os, const Ticket& t);
};
