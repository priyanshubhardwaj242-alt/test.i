CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
LIB_SRC  := $(filter-out src/main.cpp,$(wildcard src/*.cpp))

all: parking

parking: $(LIB_SRC) src/main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test_runner: $(LIB_SRC) tests/test_parking.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test: test_runner
	./test_runner

run: parking
	./parking

clean:
	rm -rf parking test_runner test_data

.PHONY: all test run clean
