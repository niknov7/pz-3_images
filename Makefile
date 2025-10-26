TARGET = pz1-classes

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

BUILD_DIR = build

SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC))

ifeq ($(OS),Windows_NT)
    MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
    EXE = $(TARGET).exe
else
    MKDIR = mkdir -p $(BUILD_DIR)
    EXE = $(TARGET)
endif

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

$(BUILD_DIR)/%.o: %.cpp
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	-if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
	-if exist $(EXE) del /F /Q $(EXE)
else
	-rm -rf $(BUILD_DIR) $(EXE)
endif

rebuild: clean all

.PHONY: all clean rebuild
