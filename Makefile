CXX := g++
CXXFLAGS := -std=c++17 -Wall

SRC_DIR := src
BUILD_DIR := build
TEST_DIR := tests

EXECUTABLE := exec
TEST_EXECUTABLE := test

# Generates a list of all .cpp files in given dir
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
SRCS_NO_MAIN := $(filter-out $(SRC_DIR)/main.cpp, $(SRCS))

# Generates a list of .o files in BUILD_DIR for all input files generated via SRCS
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
OBJS_NO_MAIN := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS_NO_MAIN))
TEST_OBJS:= $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TEST_SRCS))


# $<: First prereq (ie src/main.cpp)
# $@: Name of output (ie build/main.o)
# Compiles .cpp into corresponding .o if .cpp has changed
# Activated by OBJS dependency
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# $< all dependencies (ie all $(BUILD_DIR)/%.o)
# Links all .o files in build to $(EXECUTABLE)
$(EXECUTABLE) : $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXECUTABLE): $(SRCS_NO_MAIN) $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@
clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE) $(TEST_EXECUTABLE)
