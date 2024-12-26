CXX := g++
CXXFLAGS := -std=c++17 -Wall

SRC_DIR := src
BUILD_DIR := build
EXECUTABLE := exec.exe

# Generates a list of input files (all .cpp files in SRC_DIR)
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# Generates a list of .o files in BUILD_DIR for all input files generated via SRCS
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# $<: First prereq (ie src/main.cpp)
# $@: Name of output (ie build/main.o)
# Compiles .cpp into corresponding .o if .cpp has changed
# Activated by OBJS dependency
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# $< all dependencies (ie all $(BUILD_DIR)/%.o)
# Links all .o files in build to $(EXECUTABLE)
exec : $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $(EXECUTABLE)




