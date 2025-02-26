INCLUDE_DIR := ./include
CXX := g++
CXXFLAGS := -std=c++17 -Wall -I$(INCLUDE_DIR)

SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
INCLUDE_DIR := include
TEST_DIR := tests

COMMON_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
COMMON_OBJS := $(COMMON_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Connect Four Module Rules --------------------------
C4_SRCS := $(wildcard $(SRC_DIR)/connectFour/*.cpp)
C4_OBJS := $(C4_SRCS:$(SRC_DIR)/connectFour/%.cpp=$(BUILD_DIR)/connectFour/%.o)
C4_EXE := $(BIN_DIR)/connectFourEntry

# Test Module Rules ----------------------------------
ENTRY_FILES := $(SRC_DIR)/connectFour/ConnectFourMain.cpp
TESTABLE_SRCS := $(filter-out $(ENTRY_FILES), $(COMMON_SRCS) $(C4_SRCS))
TESTABLE_OBJS := $(TESTABLE_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

TEST_EXE := $(BIN_DIR)/test
TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS := $(TEST_SRCS:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/%.o)

all: $(C4_EXE) $(TEST_EXE)

connectFour: $(C4_EXE)

test: $(TEST_EXE)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(C4_EXE): $(C4_OBJS) $(COMMON_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_EXE): $(TESTABLE_OBJS) $(TEST_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
