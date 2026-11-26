#include <cassert>
#include <filesystem>
#include <iostream>
#include "Exceptions.h"
#include "ParkingLot.h"

static int passed = 0;
#define CHECK(cond) do { assert(cond); ++passed; } while (0)
#define CHECK_THROWS(expr, ExType) do { bool ok = false; \
    try { expr; } catch (const ExType&) { ok = true; } assert(ok); ++passed; } while (0)

int main() {
    namespace fs = std::filesystem;
    fs::remove_all("test_data");
    const std::time_t t0 = 1'700'000'000;

    {   // basic park / unpark and billing
        ParkingLot lot(1, 1, 1, "test_data");
        Ticket t = lot.park(VehicleType::CAR, "rj14 ab 1234", "Asha", t0);
        CHECK(t.slotId() == 2);
        CHECK(lot.freeSlots(VehicleType::CAR) == 0);
        CHECK(lot.find("RJ14 AB 1234") != nullptr);
        Ticket r = lot.unpark("RJ14 AB 1234", t0 + 90 * 60);  // 90 min -> 2 hr
        CHECK(r.hours() == 2);
        CHECK(r.fee() == 40.0);
        CHECK(lot.freeSlots(VehicleType::CAR) == 1);
        CHECK(lot.revenue() == 40.0);
    }
    {   // minimum 1 hour, different rates (polymorphism)
        ParkingLot lot(1, 1, 1, "test_data");
        lot.park(VehicleType::BIKE, "BIKE0001", "Ravi", t0);
        lot.park(VehicleType::TRUCK, "TRUCK001", "Meena", t0);
        CHECK(lot.unpark("BIKE0001", t0 + 60).fee() == 10.0);
        CHECK(lot.unpark("TRUCK001", t0 + 3 * 3600).fee() == 150.0);
    }
    {   // error handling
        ParkingLot lot(1, 1, 1, "test_data");
        lot.park(VehicleType::CAR, "CAR-0001", "A", t0);
        CHECK_THROWS(lot.park(VehicleType::CAR, "CAR-0001", "A", t0), DuplicateVehicleException);
        CHECK_THROWS(lot.park(VehicleType::CAR, "CAR-0002", "B", t0), LotFullException);
        CHECK_THROWS(lot.unpark("NOPE1234"), VehicleNotFoundException);
        CHECK_THROWS(lot.park(VehicleType::BIKE, "ab", "C", t0), InvalidInputException);
        CHECK_THROWS(lot.park(VehicleType::BIKE, "BIKE0009", "  ", t0), InvalidInputException);
    }
    {   // persistence round trip
        fs::remove_all("test_data");
        ParkingLot a(2, 2, 2, "test_data");
        a.park(VehicleType::CAR, "SAVE0001", "Neha", t0);
        a.save();
        ParkingLot b(2, 2, 2, "test_data");
        CHECK(b.load());
        CHECK(b.activeCount() == 1);
        CHECK(b.find("SAVE0001") != nullptr);
        CHECK(b.freeSlots(VehicleType::CAR) == 1);
    }
    fs::remove_all("test_data");
    std::cout << "All " << passed << " checks passed.\n";
    return 0;
}
