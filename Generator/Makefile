CC = clang++
CXXFLAGS = -std=c++20

SRC_DIR = ./src
OBJ_DIR = ./obj

SRCS = $(notdir $(wildcard $(SRC_DIR)/*.cpp))
OBJS = $(SRCS:.cpp=.o)

OBJECTS = $(patsubst %.o,$(OBJ_DIR)/%.o,$(OBJS))
DEPS = $(OBJECTS:.o=.d)

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(CXXFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(SRC_DIR) $(OBJ_DIR)
	$(CC) -c $< -o $@ -MD $(CXXFLAGS)

.PHONY: clean all

clean:
	rm -f $(OBJECTS) $(DEPS) $(TARGET)

-include $(DEPS)