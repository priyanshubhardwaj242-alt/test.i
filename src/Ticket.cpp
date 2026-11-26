#include "Ticket.h"
#include <cmath>
#include <iomanip>
#include "Utils.h"

Ticket::Ticket(int id, std::string plate, std::string owner, VehicleType type,
               int slotId, std::time_t entry, double rate)
    : id_(id), plate_(std::move(plate)), owner_(std::move(owner)), type_(type),
      slotId_(slotId), entry_(entry), rate_(rate) {}

void Ticket::close(std::time_t exitTime) {
    exit_ = exitTime;
    double secs = std::difftime(exit_, entry_);
    if (secs < 0) secs = 0;
    hours_ = static_cast<int>(std::ceil(secs / 3600.0));
    if (hours_ < 1) hours_ = 1;
    fee_ = hours_ * rate_;
}

std::ostream& operator<<(std::ostream& os, const Ticket& t) {
    os << "---------------- PARKING TICKET ----------------\n"
       << " Ticket No : " << t.id_ << "\n"
       << " Vehicle   : " << toString(t.type_) << " " << t.plate_ << "\n"
       << " Owner     : " << t.owner_ << "\n"
       << " Slot      : " << t.slotId_ << "\n"
       << " Entry     : " << util::formatTime(t.entry_) << "\n";
    if (t.exit_ != 0) {
        os << " Exit      : " << util::formatTime(t.exit_) << "\n"
           << " Duration  : " << t.hours_ << " hr (billed)\n"
           << " Rate      : Rs. " << std::fixed << std::setprecision(2) << t.rate_
           << "/hr\n"
           << " TOTAL FEE : Rs. " << t.fee_ << "\n";
    }
    os << "------------------------------------------------\n";
    return os;
}
