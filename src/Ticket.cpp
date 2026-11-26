#include "Ticket.h"
#include "Exceptions.h"
#include "Utils.h"
#include <cmath>
#include <iomanip>
#include <vector>

int Ticket::nextId_ = 1;

Ticket::Ticket(int id, std::string plate, VehicleType type, std::string slot,
               double rate, std::time_t entry, std::time_t exit, double fee)
    : id_(id), plate_(std::move(plate)), type_(type), slot_(std::move(slot)),
      rate_(rate), entry_(entry), exit_(exit), fee_(fee) {}

Ticket::Ticket(const std::string& plate, VehicleType type, const std::string& slot,
               double rate, std::time_t entry)
    : Ticket(nextId_++, plate, type, slot, rate, entry, 0, 0.0) {}

long Ticket::billableHours(std::time_t until) const {
    double hours = std::difftime(until, entry_) / 3600.0;
    long billable = static_cast<long>(std::ceil(hours));
    return billable < 1 ? 1 : billable;
}

void Ticket::close(std::time_t exitTime) {
    if (isClosed()) throw ParkingException("Ticket is already closed.");
    if (exitTime < entry_) throw ParkingException("Exit time is before entry time.");
    exit_ = exitTime;
    fee_ = static_cast<double>(billableHours(exit_)) * rate_;
}

std::string Ticket::toRecord() const {
    std::ostringstream os;
    os << id_ << ',' << plate_ << ',' << toString(type_) << ',' << slot_ << ','
       << rate_ << ',' << entry_ << ',' << exit_ << ',' << fee_;
    return os.str();
}

Ticket Ticket::fromRecord(const std::string& line) {
    std::vector<std::string> f;
    std::istringstream ss(line);
    for (std::string part; std::getline(ss, part, ',');) f.push_back(part);
    if (f.size() != 8) throw ParkingException("Corrupt record: " + line);
    try {
        return Ticket(std::stoi(f[0]), f[1], vehicleTypeFromString(f[2]), f[3],
                      std::stod(f[4]), static_cast<std::time_t>(std::stoll(f[5])),
                      static_cast<std::time_t>(std::stoll(f[6])), std::stod(f[7]));
    } catch (const std::logic_error&) {
        throw ParkingException("Corrupt record: " + line);
    }
}

std::ostream& operator<<(std::ostream& os, const Ticket& t) {
    os << "----------- PARKING TICKET -----------\n"
       << "Ticket no : " << t.id_ << "\n"
       << "Vehicle   : " << toString(t.type_) << " " << t.plate_ << "\n"
       << "Slot      : " << t.slot_ << "\n"
       << "Rate      : Rs " << std::fixed << std::setprecision(2) << t.rate_ << " / hour\n"
       << "Entry     : " << formatTime(t.entry_) << "\n";
    if (t.isClosed()) {
        os << "Exit      : " << formatTime(t.exit_) << "\n"
           << "Hours     : " << t.billableHours(t.exit_) << "\n"
           << "Total fee : Rs " << t.fee_ << "\n";
    }
    os << "--------------------------------------";
    return os;
}
