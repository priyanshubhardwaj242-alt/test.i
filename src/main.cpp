#include "ParkingLot.h"
#include "Exceptions.h"
#include "Utils.h"
#include <iostream>

namespace {
const char* kDataFile = "parking_data.csv";

void printMenu() {
    std::cout << "\n===== PARKING MANAGEMENT SYSTEM =====\n"
              << " 1. Park a vehicle\n"
              << " 2. Exit a vehicle and pay\n"
              << " 3. Show slot status\n"
              << " 4. Search a vehicle\n"
              << " 5. Revenue report\n"
              << " 6. Save and quit\n";
}

void parkVehicle(ParkingLot& lot) {
    int choice = readValue<int>("Vehicle type (1 Bike, 2 Car, 3 Truck): ");
    if (choice < 1 || choice > 3) throw ParkingException("Choose 1, 2 or 3.");
    auto type = static_cast<VehicleType>(choice - 1);
    auto vehicle = makeVehicle(type, readLine("Registration number: "));
    std::cout << lot.park(*vehicle) << "\n";
}

void exitVehicle(ParkingLot& lot) {
    std::string plate = Vehicle::normalizePlate(readLine("Registration number: "));
    std::cout << lot.exitVehicle(plate) << "\n";
}

void searchVehicle(const ParkingLot& lot) {
    std::string plate = Vehicle::normalizePlate(readLine("Registration number: "));
    if (const Ticket* t = lot.find(plate)) std::cout << *t << "\n";
    else std::cout << plate << " is not parked here.\n";
}
}  // namespace

int main() {
    ParkingLot lot(5, 10, 3);  // 5 bike, 10 car, 3 truck slots
    try {
        lot.load(kDataFile);
    } catch (const ParkingException& e) {
        std::cerr << "Warning: " << e.what() << "\n";
    }

    bool running = true;
    while (running) {
        try {
            printMenu();
            switch (readValue<int>("Choose an option: ")) {
                case 1: parkVehicle(lot); break;
                case 2: exitVehicle(lot); break;
                case 3: lot.printStatus(std::cout); break;
                case 4: searchVehicle(lot); break;
                case 5: lot.printHistory(std::cout); break;
                case 6: running = false; break;
                default: std::cout << "Please choose 1-6.\n";
            }
        } catch (const ParkingException& e) {
            std::cout << "Error: " << e.what() << "\n";
        } catch (const InputClosed&) {
            running = false;
        }
    }

    try {
        lot.save(kDataFile);
        std::cout << "\nData saved. Goodbye!\n";
    } catch (const ParkingException& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
