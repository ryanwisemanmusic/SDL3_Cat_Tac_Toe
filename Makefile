# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
OBJCPPFLAGS = $(CXXFLAGS) -ObjC++

# SDL3 paths
SDL3_INCLUDE = /opt/homebrew/Cellar/sdl3/3.2.8/include
SDL3_LIB = /opt/homebrew/Cellar/sdl3/3.2.8/lib

# Header directories
HEADER = -isystem $(SDL3_INCLUDE) \
         -Iinclude/cpp_headers \
         -Iinclude/objc_headers \
         -Isrc/objc

# Library flags
LIB_FLAGS = -L$(SDL3_LIB) -lSDL3 -framework Cocoa -lobjc

# Target and sources
TARGET = AtaraxiaSDK
SRC_CPP = src/cpp/main.cpp
SRC_OBJC = src/objc/cocoaToolbarUI.mm

# Object files
OBJ_CPP = $(SRC_CPP:.cpp=.o)
OBJ_OBJC = $(SRC_OBJC:.mm=.o)
OBJS = $(OBJ_CPP) $(OBJ_OBJC)

# Build rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIB_FLAGS) -o $@

src/cpp/%.o: src/cpp/%.cpp
	$(CXX) $(CXXFLAGS) $(HEADER) -c $< -o $@

src/objc/%.o: src/objc/%.mm
	$(CXX) $(OBJCPPFLAGS) $(HEADER) -c $< -o $@

# Utilities
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean run