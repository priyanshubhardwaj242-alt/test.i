CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Iinclude
LIB      := src/Vehicle.cpp src/ParkingSlot.cpp src/Ticket.cpp src/ParkingLot.cpp

all: parking

parking: $(LIB) src/main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test_parking: $(LIB) tests/test_parking.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test: test_parking
	./test_parking

run: parking
	./parking

clean:
	rm -f parking test_parking parking_data.csv test_parking_data.csv *.exe

.PHONY: all test run clean
