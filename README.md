# Parking Management System (C++)

![Build](https://github.com/<your-username>/parking-management-system/actions/workflows/build.yml/badge.svg)

A console-based **Parking Management System** written in modern C++17 for an
Object Oriented Programming lab. It manages a lot with separate slots for
bikes, cars and trucks, issues tickets, calculates fees, and saves its state
to disk so data survives between runs.

## Features

- Park a vehicle (automatic slot allocation by vehicle type)
- Remove a vehicle and generate a receipt with the fee
- Search a vehicle by plate number
- View all parked vehicles and live slot status
- Revenue report and CSV history of completed sessions
- Persistent storage (state is reloaded on next start)
- Input validation and custom exceptions

## Billing rule

Time is rounded **up** to the next hour, with a minimum of 1 hour.

| Vehicle | Rate        |
|---------|-------------|
| Bike    | Rs. 10 / hr |
| Car     | Rs. 20 / hr |
| Truck   | Rs. 50 / hr |

Default lot: 5 bike slots, 10 car slots, 3 truck slots (change in `src/main.cpp`).

## OOP concepts demonstrated

| Concept | Where |
|---|---|
| Abstraction / abstract class | `Vehicle` (pure virtual `type()` and `ratePerHour()`) |
| Inheritance | `Bike`, `Car`, `Truck` derive from `Vehicle`; custom exceptions derive from `ParkingException` |
| Polymorphism | `Vehicle::ratePerHour()` / `describe()` called through `shared_ptr<Vehicle>` |
| Encapsulation | Private data in `ParkingSlot`, `Ticket`, `ParkingLot` with public accessors |
| Operator overloading | `operator<<` for `Ticket` prints the receipt |
| Exception handling | `LotFullException`, `DuplicateVehicleException`, `VehicleNotFoundException`, `InvalidInputException` |
| Factory function | `makeVehicle()` |
| File handling | `ParkingLot::save()/load()` and `history.csv` |
| STL | `vector`, `map`, `shared_ptr`, `<filesystem>` |

## Class overview

```
Vehicle (abstract)
 ├── Bike
 ├── Car
 └── Truck

ParkingSlot  --holds-->  Vehicle
Ticket       (entry/exit time, fee)
ParkingLot   --has many-->  ParkingSlot,  --tracks-->  Ticket
```

## Project structure

```
parking-management-system/
├── include/        header files (.h)
├── src/            implementation (.cpp) and main.cpp
├── tests/          unit tests (assert based)
├── .github/workflows/build.yml   GitHub Actions CI
├── Makefile
└── README.md
```

## Build and run

Requires a C++17 compiler (g++ 8+ / clang / MSVC).

```bash
make          # build
./parking     # run  (Windows: parking.exe)
make test     # run unit tests
make clean
```

Without `make`:

```bash
g++ -std=c++17 -Iinclude src/*.cpp -o parking
```

Runtime data is stored in a `data/` folder (`state.dat`, `history.csv`).

## Sample session

```
===== PARKING MANAGEMENT SYSTEM =====
 1. Park a vehicle
 ...
Enter choice: 1
  1) Bike   2) Car   3) Truck
  Vehicle type: 2
  Plate number: RJ14 AB 1234
  Owner name  : Asha

Vehicle parked successfully!
---------------- PARKING TICKET ----------------
 Ticket No : 1
 Vehicle   : Car RJ14 AB 1234
 Owner     : Asha
 Slot      : 6
 Entry     : 2026-11-26 10:30
------------------------------------------------
```

## Push to GitHub

```bash
cd parking-management-system
git init
git add .
git commit -m "Initial commit: Parking Management System"
git branch -M main
git remote add origin https://github.com/<your-username>/parking-management-system.git
git push -u origin main
```

## Author

Priyanshu Bhardwaj

## License

MIT
