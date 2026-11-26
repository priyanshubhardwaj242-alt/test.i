#include <iostream>
#include <limits>
#include <string>
#include "Exceptions.h"
#include "ParkingLot.h"

static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    if (!std::getline(std::cin, s)) { std::cout << "\n"; std::exit(0); }
    return s;
}

static int readInt(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        try {
            std::size_t pos;
            int v = std::stoi(s, &pos);
            if (pos == s.size()) return v;
        } catch (...) {}
        std::cout << "Please enter a valid number.\n";
    }
}

static VehicleType askType() {
    while (true) {
        std::cout << "  1) Bike   2) Car   3) Truck\n";
        int c = readInt("  Vehicle type: ");
        if (c == 1) return VehicleType::BIKE;
        if (c == 2) return VehicleType::CAR;
        if (c == 3) return VehicleType::TRUCK;
        std::cout << "Choose 1, 2 or 3.\n";
    }
}

static void menu() {
    std::cout << "\n===== PARKING MANAGEMENT SYSTEM =====\n"
                 " 1. Park a vehicle\n"
                 " 2. Remove vehicle & pay\n"
                 " 3. Search vehicle\n"
                 " 4. Show parked vehicles\n"
                 " 5. Show slot status\n"
                 " 6. Revenue & history\n"
                 " 0. Save & exit\n";
}

int main() {
    ParkingLot lot(5, 10, 3);  // 5 bike, 10 car, 3 truck slots

    try {
        if (lot.load())
            std::cout << "Previous session restored (" << lot.activeCount()
                      << " vehicle(s) parked).\n";
    } catch (const ParkingException& e) {
        std::cout << "Warning: " << e.what() << " Starting fresh.\n";
    }

    while (true) {
        menu();
        int choice = readInt("Enter choice: ");
        try {
            switch (choice) {
                case 1: {
                    VehicleType t = askType();
                    std::string plate = readLine("  Plate number: ");
                    std::string owner = readLine("  Owner name  : ");
                    Ticket tk = lot.park(t, plate, owner);
                    std::cout << "\nVehicle parked successfully!\n" << tk;
                    lot.save();
                    break;
                }
                case 2: {
                    std::string plate = readLine("  Plate number: ");
                    Ticket tk = lot.unpark(plate);
                    std::cout << "\nVehicle exited. Receipt:\n" << tk;
                    lot.save();
                    break;
                }
                case 3: {
                    std::string plate = readLine("  Plate number: ");
                    if (const Ticket* tk = lot.find(plate)) std::cout << *tk;
                    else std::cout << "Vehicle not found.\n";
                    break;
                }
                case 4: lot.printActive(std::cout); break;
                case 5: lot.printStatus(std::cout); break;
                case 6: lot.printHistory(std::cout); break;
                case 0:
                    lot.save();
                    std::cout << "Data saved. Goodbye!\n";
                    return 0;
                default: std::cout << "Invalid choice.\n";
            }
        } catch (const ParkingException& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}
