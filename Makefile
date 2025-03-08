# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
OBJCPPFLAGS = $(CXXFLAGS) -ObjC++

# SDL3 paths
SDL3_INCLUDE := /opt/homebrew/Cellar/sdl3/3.2.8/include
SDL3_LIB := /opt/homebrew/Cellar/sdl3/3.2.8/lib

# SDL3_image paths
SDL3_IMAGE_INCLUDE := /opt/homebrew/Cellar/sdl3_image/3.2.4/include
SDL3_IMAGE_LIB := /opt/homebrew/Cellar/sdl3_image/3.2.4/lib

# SDL3_ttf paths
SDL3_TTF_INCLUDE := /usr/local/include/SDL3_ttf
SDL3_TTF_LIB := /usr/local/lib

SQLITE_INCLUDE := /opt/homebrew/Cellar/sqlite/3.49.1/include
SQLITE_LIB := /opt/homebrew/Cellar/sqlite/3.49.1/lib

# Header directories
HEADER = -isystem $(SDL3_INCLUDE) \
         -I$(SDL3_IMAGE_INCLUDE) \
         -I$(SDL3_TTF_INCLUDE) \
         -Iinclude/cpp_headers \
         -Iinclude/objc_headers \
         -Isrc/objc \
         -Idatabase \
         -I$(SQLITE_INCLUDE)

# Library flags
LIB_FLAGS = -L$(SDL3_LIB) -L$(SDL3_IMAGE_LIB) -L$(SDL3_TTF_LIB) \
            -L$(SQLITE_LIB) \
            -lSDL3 -lSDL3_image -lSDL3_ttf \
            -framework Cocoa -lobjc -framework OpenGL  \
            -rpath /usr/local/lib 

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
	export DYLD_LIBRARY_PATH="/usr/local/lib:$$DYLD_LIBRARY_PATH" && ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean run
