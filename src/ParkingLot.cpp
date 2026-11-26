#include "ParkingLot.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include "Exceptions.h"
#include "Utils.h"

namespace fs = std::filesystem;

ParkingLot::ParkingLot(int bikeSlots, int carSlots, int truckSlots,
                       std::string dataDir)
    : dataDir_(std::move(dataDir)) {
    int id = 1;
    for (int i = 0; i < bikeSlots; ++i)  slots_.emplace_back(id++, VehicleType::BIKE);
    for (int i = 0; i < carSlots; ++i)   slots_.emplace_back(id++, VehicleType::CAR);
    for (int i = 0; i < truckSlots; ++i) slots_.emplace_back(id++, VehicleType::TRUCK);
}

ParkingSlot* ParkingLot::findFreeSlot(VehicleType t) {
    for (auto& s : slots_)
        if (s.type() == t && s.isFree()) return &s;
    return nullptr;
}

ParkingSlot* ParkingLot::slotById(int id) {
    for (auto& s : slots_)
        if (s.id() == id) return &s;
    return nullptr;
}

Ticket ParkingLot::park(VehicleType type, const std::string& rawPlate,
                        const std::string& rawOwner, std::time_t at) {
    const std::string plate = util::normalizePlate(rawPlate);
    const std::string owner = util::validateOwner(rawOwner);

    if (active_.count(plate)) throw DuplicateVehicleException(plate);

    ParkingSlot* slot = findFreeSlot(type);
    if (!slot) throw LotFullException(toString(type));

    auto vehicle = makeVehicle(type, plate, owner);  // polymorphic object
    slot->park(vehicle);

    Ticket t(nextTicketId_++, plate, owner, type, slot->id(), at,
             vehicle->ratePerHour());
    active_.emplace(plate, t);
    return t;
}

Ticket ParkingLot::unpark(const std::string& rawPlate, std::time_t at) {
    const std::string plate = util::normalizePlate(rawPlate);
    auto it = active_.find(plate);
    if (it == active_.end()) throw VehicleNotFoundException(plate);

    Ticket t = it->second;
    t.close(at);
    if (ParkingSlot* s = slotById(t.slotId())) s->vacate();
    revenue_ += t.fee();
    active_.erase(it);

    // Append to the history log (CSV)
    fs::create_directories(dataDir_);
    const std::string path = dataDir_ + "/history.csv";
    const bool isNew = !fs::exists(path);
    std::ofstream out(path, std::ios::app);
    if (isNew)
        out << "ticket,plate,owner,type,slot,entry,exit,hours,fee\n";
    out << t.id() << ',' << t.plate() << ',' << t.owner() << ','
        << toString(t.type()) << ',' << t.slotId() << ','
        << util::formatTime(t.entry()) << ',' << util::formatTime(t.exit()) << ','
        << t.hours() << ',' << std::fixed << std::setprecision(2) << t.fee() << '\n';
    return t;
}

const Ticket* ParkingLot::find(const std::string& rawPlate) const {
    std::string plate;
    try { plate = util::normalizePlate(rawPlate); }
    catch (const InvalidInputException&) { return nullptr; }
    auto it = active_.find(plate);
    return it == active_.end() ? nullptr : &it->second;
}

int ParkingLot::totalSlots(VehicleType t) const {
    int n = 0;
    for (const auto& s : slots_) if (s.type() == t) ++n;
    return n;
}

int ParkingLot::freeSlots(VehicleType t) const {
    int n = 0;
    for (const auto& s : slots_) if (s.type() == t && s.isFree()) ++n;
    return n;
}

void ParkingLot::printStatus(std::ostream& os) const {
    os << "\n=========== SLOT STATUS ===========\n";
    for (VehicleType t : {VehicleType::BIKE, VehicleType::CAR, VehicleType::TRUCK}) {
        os << std::left << std::setw(6) << toString(t) << " slots: "
           << freeSlots(t) << " free / " << totalSlots(t) << " total\n  ";
        for (const auto& s : slots_) {
            if (s.type() != t) continue;
            os << "[" << std::setw(2) << s.id() << (s.isFree() ? " free" : " FULL") << "] ";
        }
        os << "\n";
    }
    os << "===================================\n";
}

void ParkingLot::printActive(std::ostream& os) const {
    if (active_.empty()) { os << "\nNo vehicles are currently parked.\n"; return; }
    os << "\n" << std::left << std::setw(8) << "Ticket" << std::setw(14) << "Plate"
       << std::setw(8) << "Type" << std::setw(6) << "Slot" << std::setw(18) << "Owner"
       << "Entry\n" << std::string(68, '-') << "\n";
    for (const auto& [plate, t] : active_) {
        os << std::setw(8) << t.id() << std::setw(14) << plate << std::setw(8)
           << toString(t.type()) << std::setw(6) << t.slotId() << std::setw(18)
           << t.owner() << util::formatTime(t.entry()) << "\n";
    }
}

void ParkingLot::printHistory(std::ostream& os) const {
    os << "\nTotal revenue collected: Rs. " << std::fixed << std::setprecision(2)
       << revenue_ << "\n";
    std::ifstream in(dataDir_ + "/history.csv");
    if (!in) { os << "No completed parking sessions yet.\n"; return; }
    os << "\n--- Completed sessions ---\n";
    std::string line;
    while (std::getline(in, line)) os << line << "\n";
}

// ---------- persistence ----------
void ParkingLot::save() const {
    fs::create_directories(dataDir_);
    std::ofstream out(dataDir_ + "/state.dat", std::ios::trunc);
    if (!out) throw ParkingException("Could not write state file.");
    out << "NEXT|" << nextTicketId_ << "\n";
    out << "REVENUE|" << std::fixed << std::setprecision(2) << revenue_ << "\n";
    for (const auto& [plate, t] : active_) {
        out << "T|" << t.id() << '|' << t.plate() << '|' << t.owner() << '|'
            << static_cast<int>(t.type()) << '|' << t.slotId() << '|'
            << static_cast<long long>(t.entry()) << "\n";
    }
}

bool ParkingLot::load() {
    std::ifstream in(dataDir_ + "/state.dat");
    if (!in) return false;
    try {
        std::string line;
        while (std::getline(in, line)) {
            auto f = util::split(line, '|');
            if (f.empty()) continue;
            if (f[0] == "NEXT" && f.size() == 2) {
                nextTicketId_ = std::stoi(f[1]);
            } else if (f[0] == "REVENUE" && f.size() == 2) {
                revenue_ = std::stod(f[1]);
            } else if (f[0] == "T" && f.size() == 7) {
                int id = std::stoi(f[1]);
                auto type = static_cast<VehicleType>(std::stoi(f[4]));
                int slotId = std::stoi(f[5]);
                std::time_t entry = static_cast<std::time_t>(std::stoll(f[6]));
                ParkingSlot* slot = slotById(slotId);
                if (!slot || slot->type() != type || !slot->isFree())
                    throw ParkingException("Saved data does not match lot layout.");
                auto v = makeVehicle(type, f[2], f[3]);
                slot->park(v);
                active_.emplace(f[2], Ticket(id, f[2], f[3], type, slotId, entry,
                                             v->ratePerHour()));
            }
        }
    } catch (const ParkingException&) {
        throw;
    } catch (const std::exception&) {
        throw ParkingException("State file is corrupt.");
    }
    return true;
}
