# Parking Management System (C++)

[![Build and test](https://github.com/priyanshubhardwaj242-alt/parking-management-system/actions/workflows/build.yml/badge.svg)](https://github.com/priyanshubhardwaj242-alt/parking-management-system/actions/workflows/build.yml)

**Author:** Priyanshu Bhardwaj  
**Course:** Object Oriented Programming Lab

A console application written in **C++17** that manages a parking lot with separate slots for bikes, cars and trucks. It issues tickets, calculates fees, and saves its data to a file so nothing is lost between runs.

## Features

- Park a vehicle: finds the first free slot of the right type and prints a ticket
- Exit a vehicle: calculates the fee (hours rounded up, minimum 1 hour) and frees the slot
- Live slot status, search by registration number, and a revenue report
- Registration numbers are validated and normalised, so `ka 01-ab 1234` becomes `KA01AB1234`
- Data is saved to `parking_data.csv` and reloaded on the next run
- A data file that does not match the lot layout is never half-loaded: the old file is kept as `parking_data.csv.bak` and the program starts with an empty lot
- Unit tests, plus a GitHub Actions workflow that builds and tests on every push

## Rates and slots

- **Bike:** Rs 10 per hour, 5 slots (B1 to B5)
- **Car:** Rs 20 per hour, 10 slots (C1 to C10)
- **Truck:** Rs 40 per hour, 3 slots (T1 to T3)

The slot counts are set in one line at the top of `main()` in `src/main.cpp`.

## OOP concepts used

- **Classes and encapsulation:** `ParkingSlot`, `Ticket` and `ParkingLot` keep their data private and expose methods
- **Abstract class and pure virtual functions:** `Vehicle::type()` and `Vehicle::ratePerHour()`
- **Inheritance:** `Bike`, `Car` and `Truck` derive from `Vehicle`
- **Polymorphism:** `ParkingLot::park()` calls `ratePerHour()` through a base-class reference
- **Virtual destructor and smart pointers:** `Vehicle`, and `std::unique_ptr` in `makeVehicle()`
- **Constructor delegation:** the private and public `Ticket` constructors
- **Static data member:** `Ticket::nextId_` generates ticket numbers
- **Operator overloading:** `operator<<` prints a `Ticket`
- **Exception handling:** the custom `ParkingException` is thrown for every rule violation
- **Function template:** `readValue<T>()` in `Utils.h`
- **File handling:** `ParkingLot::save()` and `load()` use a CSV file
- **STL:** `vector`, `map`, `algorithm`, `string` and `sstream`

## Class structure

```
Vehicle (abstract class)
 |-- Bike    (Rs 10 per hour)
 |-- Car     (Rs 20 per hour)
 `-- Truck   (Rs 40 per hour)

ParkingLot
 |-- has many ParkingSlot   (B1..B5, C1..C10, T1..T3)
 |-- has many Ticket        (open tickets and history)
 `-- uses Vehicle           (to park and to exit)
```

## Project structure

```
parking-management-system/
|-- include/             header files
|-- src/                 implementation files and main.cpp (menu)
|-- tests/               test_parking.cpp (unit tests)
|-- .github/workflows/   build.yml (GitHub Actions)
|-- Makefile
|-- LICENSE
`-- README.md
```

## Build and run

You need a C++17 compiler (g++ or clang++).

```bash
git clone https://github.com/priyanshubhardwaj242-alt/parking-management-system.git
cd parking-management-system
```

With make (Linux, macOS, or Windows with MinGW or WSL):

```bash
make          # build the program
make run      # build and run
make test     # build and run the unit tests
make clean
```

Without make (any system with g++):

```bash
g++ -std=c++17 -Iinclude src/Vehicle.cpp src/ParkingSlot.cpp src/Ticket.cpp src/ParkingLot.cpp src/main.cpp -o parking
./parking
```

On Windows the program is `parking.exe`.

## Sample run

Parking a car, then taking it out again:

```
Choose an option: 1
Vehicle type (1 Bike, 2 Car, 3 Truck): 2
Registration number: ka 01-ab 1234
----------- PARKING TICKET -----------
Ticket no : 1
Vehicle   : Car KA01AB1234
Slot      : C1
Rate      : Rs 20.00 / hour
Entry     : 2026-11-26 04:01
--------------------------------------

Choose an option: 2
Registration number: KA01AB1234
----------- PARKING TICKET -----------
Ticket no : 1
Vehicle   : Car KA01AB1234
Slot      : C1
Rate      : Rs 20.00 / hour
Entry     : 2026-11-26 04:01
Exit      : 2026-11-26 04:01
Hours     : 1
Total fee : Rs 20.00
--------------------------------------
```

## Tests

`make test` runs `tests/test_parking.cpp`, which checks:

- registration number validation and normalisation
- the different hourly rates through the base class (polymorphism)
- fee rounding (1 minute is billed as 1 hour, 2 hours 1 second as 3 hours) and revenue totals
- the rules: duplicate vehicle, full lot, unknown vehicle, exit before entry, no slot of that type
- saving and loading, including that ticket numbers continue after a reload
- a data file that does not match the lot leaves the lot untouched

## Possible extensions

Multiple floors, rates that change by time of day, monthly pass holders, and a graphical or web front end.

## License

MIT, see the `LICENSE` file.
