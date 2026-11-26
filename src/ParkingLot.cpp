#include "ParkingLot.h"
#include "Exceptions.h"
#include "Utils.h"
#include <algorithm>
#include <fstream>
#include <iomanip>

namespace {
char prefix(VehicleType t) {
    switch (t) {
        case VehicleType::Bike:  return 'B';
        case VehicleType::Car:   return 'C';
        case VehicleType::Truck: return 'T';
    }
    return '?';
}
const VehicleType kTypes[] = {VehicleType::Bike, VehicleType::Car, VehicleType::Truck};
}  // namespace

ParkingLot::ParkingLot(int bikeSlots, int carSlots, int truckSlots) {
    if (bikeSlots < 0 || carSlots < 0 || truckSlots < 0)
        throw ParkingException("Slot counts cannot be negative.");
    const int counts[] = {bikeSlots, carSlots, truckSlots};
    for (int k = 0; k < 3; ++k)
        for (int i = 1; i <= counts[k]; ++i)
            slots_.emplace_back(std::string(1, prefix(kTypes[k])) + std::to_string(i), kTypes[k]);
}

ParkingSlot* ParkingLot::findFreeSlot(VehicleType type) {
    for (auto& s : slots_)
        if (s.type() == type && s.isFree()) return &s;
    return nullptr;
}

ParkingSlot* ParkingLot::findSlot(const std::string& label) {
    for (auto& s : slots_)
        if (s.label() == label) return &s;
    return nullptr;
}

Ticket ParkingLot::park(const Vehicle& vehicle, std::time_t now) {
    if (active_.count(vehicle.plate()))
        throw ParkingException(vehicle.plate() + " is already parked.");
    ParkingSlot* slot = findFreeSlot(vehicle.type());
    if (!slot)
        throw ParkingException("No free " + toString(vehicle.type()) + " slot available.");
    slot->occupy(vehicle.plate());
    // Polymorphism: ratePerHour() resolves to Bike/Car/Truck at run time.
    Ticket ticket(vehicle.plate(), vehicle.type(), slot->label(), vehicle.ratePerHour(), now);
    active_.emplace(vehicle.plate(), ticket);
    return ticket;
}

Ticket ParkingLot::exitVehicle(const std::string& plate, std::time_t now) {
    auto it = active_.find(plate);
    if (it == active_.end()) throw ParkingException(plate + " is not parked here.");
    Ticket ticket = it->second;
    ticket.close(now);
    if (ParkingSlot* s = findSlot(ticket.slot())) s->release();
    history_.push_back(ticket);
    active_.erase(it);
    return ticket;
}

const Ticket* ParkingLot::find(const std::string& plate) const {
    auto it = active_.find(plate);
    return it == active_.end() ? nullptr : &it->second;
}

int ParkingLot::totalSlots(VehicleType type) const {
    return static_cast<int>(std::count_if(slots_.begin(), slots_.end(),
        [type](const ParkingSlot& s) { return s.type() == type; }));
}

int ParkingLot::freeSlots(VehicleType type) const {
    return static_cast<int>(std::count_if(slots_.begin(), slots_.end(),
        [type](const ParkingSlot& s) { return s.type() == type && s.isFree(); }));
}

double ParkingLot::totalRevenue() const {
    double sum = 0;
    for (const auto& t : history_) sum += t.fee();
    return sum;
}

void ParkingLot::printStatus(std::ostream& os) const {
    os << "\n=========== PARKING STATUS ===========\n";
    for (VehicleType type : kTypes) {
        os << std::left << std::setw(6) << toString(type) << ": "
           << freeSlots(type) << "/" << totalSlots(type) << " free\n";
        for (const auto& s : slots_) {
            if (s.type() != type) continue;
            os << "   " << std::setw(4) << s.label()
               << (s.isFree() ? "free" : s.plate()) << "\n";
        }
    }
    os << "======================================\n";
}

void ParkingLot::printHistory(std::ostream& os) const {
    os << "\n--------- COMPLETED PARKINGS ---------\n";
    if (history_.empty()) os << "No completed parkings yet.\n";
    for (const auto& t : history_)
        os << "#" << t.id() << "  " << std::setw(12) << std::left << t.plate()
           << toString(t.type()) << "  Rs " << std::fixed << std::setprecision(2) << t.fee() << "\n";
    os << "Vehicles served: " << vehiclesServed() << "\n"
       << "Total revenue  : Rs " << std::fixed << std::setprecision(2) << totalRevenue() << "\n";
}

void ParkingLot::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) throw ParkingException("Cannot write file: " + path);
    out << "# id,plate,type,slot,rate,entry,exit,fee\n";
    for (const auto& kv : active_) out << kv.second.toRecord() << "\n";
    for (const auto& t : history_) out << t.toRecord() << "\n";
}

void ParkingLot::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) return;  // first run: nothing saved yet

    // Work on copies so a bad file leaves the lot exactly as it was.
    std::vector<ParkingSlot> slots = slots_;
    std::map<std::string, Ticket> active;
    std::vector<Ticket> history;
    int maxId = 0;

    for (std::string line; std::getline(in, line);) {
        if (line.empty() || line[0] == '#') continue;
        Ticket t = Ticket::fromRecord(line);
        maxId = std::max(maxId, t.id());
        if (t.isClosed()) {
            history.push_back(t);
            continue;
        }
        auto it = std::find_if(slots.begin(), slots.end(),
            [&t](const ParkingSlot& s) { return s.label() == t.slot(); });
        if (it == slots.end() || it->type() != t.type() || !it->isFree())
            throw ParkingException("Saved data does not match the lot layout: " + line);
        it->occupy(t.plate());
        if (!active.emplace(t.plate(), t).second)
            throw ParkingException("Saved data lists a vehicle twice: " + t.plate());
    }
    slots_.swap(slots);
    active_.swap(active);
    history_.swap(history);
    Ticket::setNextId(maxId + 1);
}
