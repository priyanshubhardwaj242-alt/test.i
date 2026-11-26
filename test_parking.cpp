#undef NDEBUG
#include "ParkingLot.h"
#include "Exceptions.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>

static const std::time_t T0 = 1700000000;

template <typename F>
static bool throwsParking(F f) {
    try { f(); } catch (const ParkingException&) { return true; }
    return false;
}

static void testPlateValidation() {
    assert(Vehicle::normalizePlate("ka 01-ab 1234") == "KA01AB1234");
    assert(throwsParking([] { Vehicle::normalizePlate("ab"); }));
    assert(throwsParking([] { Vehicle::normalizePlate("AB@1234"); }));
}

static void testPolymorphicRates() {
    Bike b("MH12AB1"); Car c("MH12AB2"); Truck t("MH12AB3");
    const Vehicle* all[] = {&b, &c, &t};
    assert(all[0]->ratePerHour() == 10 && all[1]->ratePerHour() == 20 && all[2]->ratePerHour() == 40);
}

static void testFeeCalculation() {
    ParkingLot lot(1, 1, 1);
    Car car("RJ14CD5678");
    Ticket t = lot.park(car, T0);
    assert(t.slot() == "C1");
    Ticket done = lot.exitVehicle("RJ14CD5678", T0 + 60);          // 1 minute -> 1 hour
    assert(done.fee() == 20);
    lot.park(car, T0);
    done = lot.exitVehicle("RJ14CD5678", T0 + 2 * 3600 + 1);        // 2h 1s -> 3 hours
    assert(done.fee() == 60);
    assert(lot.totalRevenue() == 80 && lot.vehiclesServed() == 2);
}

static void testRules() {
    ParkingLot lot(0, 1, 0);
    Car a("AAAA1111"), b("BBBB2222");
    lot.park(a, T0);
    assert(throwsParking([&] { lot.park(a, T0); }));                // duplicate
    assert(throwsParking([&] { lot.park(b, T0); }));                // lot full
    assert(throwsParking([&] { lot.exitVehicle("ZZZZ9999", T0); })); // unknown
    assert(throwsParking([&] { lot.exitVehicle("AAAA1111", T0 - 5); })); // time travel
    Bike bike("CCCC3333");
    assert(throwsParking([&] { lot.park(bike, T0); }));             // no bike slots
    assert(lot.freeSlots(VehicleType::Car) == 0 && lot.totalSlots(VehicleType::Car) == 1);
}

static void testSaveLoad() {
    const std::string file = "test_parking_data.csv";
    Ticket::setNextId(1);  // ids are global, so start this test from a known value
    {
        ParkingLot lot(1, 2, 1);
        Car a("AAAA1111"), b("BBBB2222");
        lot.park(a, T0);
        lot.park(b, T0);
        lot.exitVehicle("BBBB2222", T0 + 3600);
        lot.save(file);
    }
    ParkingLot copy(1, 2, 1);
    copy.load(file);
    assert(copy.vehiclesParked() == 1 && copy.vehiclesServed() == 1);
    assert(copy.find("AAAA1111") != nullptr && copy.totalRevenue() == 20);
    assert(copy.freeSlots(VehicleType::Car) == 1);
    Car c("CCCC3333");
    Ticket t = copy.park(c, T0);
    assert(t.id() == 3);  // ids continue after the saved ones
    std::remove(file.c_str());
}

static void testBadFileLeavesLotUntouched() {
    const std::string file = "test_bad_data.csv";
    {
        std::ofstream f(file);
        f << "1,AAAA1111,Car,C9,20,1700000000,0,0\n";  // slot C9 does not exist
    }
    ParkingLot lot(0, 1, 0);
    assert(throwsParking([&] { lot.load(file); }));
    assert(lot.vehiclesParked() == 0 && lot.freeSlots(VehicleType::Car) == 1);
    std::remove(file.c_str());
}

int main() {
    testPlateValidation();
    testPolymorphicRates();
    testFeeCalculation();
    testRules();
    testSaveLoad();
    testBadFileLeavesLotUntouched();
    std::cout << "All tests passed.\n";
    return 0;
}
