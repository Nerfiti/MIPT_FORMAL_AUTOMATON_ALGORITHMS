TARGET := ./Automation.out

SRC_DIR := ./src
BUILD_DIR := ./build
OBJ_DIR := $(BUILD_DIR)
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

# LDFLAGS :=
# CPPFLAGS :=

all: $(TARGET)

run: all
	$(TARGET)

clean:
	rm -f $(OBJ_FILES) $(TARGET)

$(TARGET): $(OBJ_FILES)
	g++ -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ -c -o $@ $<