# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
OBJCPPFLAGS = $(CXXFLAGS)

# Detect OS
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
    # macOS paths
    #SDL3 Paths
    SDL3_INCLUDE := /opt/homebrew/Cellar/sdl3/3.2.8/include
    SDL3_LIB := /opt/homebrew/Cellar/sdl3/3.2.8/lib
    SDL3_IMAGE_INCLUDE := /opt/homebrew/Cellar/sdl3_image/3.2.4/include
    SDL3_IMAGE_LIB := /opt/homebrew/Cellar/sdl3_image/3.2.4/lib
    #Third-party SDL3 Paths 
    SDL3_TTF_INCLUDE := /usr/local/include/SDL3_ttf
    SDL3_TTF_LIB := /usr/local/lib
    SDL3_MIXER_INCLUDE := /usr/local/include/SDL3_mixer
    SDL3_MIXER_LIB := /usr/local/lib
    #SQLite Paths
    SQLITE_INCLUDE := /opt/homebrew/Cellar/sqlite/3.49.1/include
    SQLITE_LIB := /opt/homebrew/Cellar/sqlite/3.49.1/lib
    # FFmpeg Paths
    FFMPEG_INCLUDE := /opt/homebrew/Cellar/ffmpeg/7.1.1_1/include
    FFMPEG_LIB := /opt/homebrew/Cellar/ffmpeg/7.1.1_1/lib

    # macOS-specific libraries
    PLATFORM_LIBS = -framework Cocoa -framework OpenGL -lobjc

    # macOS-specific export path
    EXPORT_LIB_PATH = export DYLD_LIBRARY_PATH="/usr/local/lib:$$DYLD_LIBRARY_PATH"
else ifeq ($(UNAME_S), Linux)
    # Linux paths
    SDL3_INCLUDE := /usr/include/SDL3
    SDL3_LIB := /usr/lib
    SDL3_IMAGE_INCLUDE := /usr/include/SDL3_image
    SDL3_IMAGE_LIB := /usr/lib
    SDL3_TTF_INCLUDE := /usr/include/SDL3_ttf
    SDL3_TTF_LIB := /usr/lib
    SQLITE_INCLUDE := /usr/include
    SQLITE_LIB := /usr/lib
    FFMPEG_INCLUDE := /usr/include/FFmpeg
    FFMPEG_LIB := /usr/lib

    # Linux-specific libraries
    PLATFORM_LIBS = -lGL -ldl -lpthread -lm

    # Linux-specific export path
    EXPORT_LIB_PATH = export LD_LIBRARY_PATH="/usr/lib:$$LD_LIBRARY_PATH"
else ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    # Windows paths (for MinGW or similar toolchain)
    SDL3_INCLUDE := C:/SDL3/include
    SDL3_LIB := C:/SDL3/lib
    SDL3_IMAGE_INCLUDE := C:/SDL3_image/include
    SDL3_IMAGE_LIB := C:/SDL3_image/lib
    SDL3_TTF_INCLUDE := C:/SDL3_ttf/include
    SDL3_TTF_LIB := C:/SDL3_ttf/lib
    SQLITE_INCLUDE := C:/sqlite/include
    SQLITE_LIB := C:/sqlite/lib

    # Windows-specific libraries
    PLATFORM_LIBS = -lSDL3 -lSDL3_image -lSDL3_ttf -lsqlite3 -lGL -lmingw32 -lSDL3main

    # Windows-specific export path (MinGW uses the same approach)
    EXPORT_LIB_PATH = set LIBRARY_PATH=%LIBRARY_PATH%;C:/SDL3/lib;C:/SDL3_image/lib;C:/SDL3_ttf/lib;C:/sqlite/lib
else
    $(error Unsupported platform)
endif

# Header directories
HEADER = -isystem $(SDL3_INCLUDE) \
         -I$(SDL3_IMAGE_INCLUDE) \
         -I$(SDL3_TTF_INCLUDE) \
         -I$(SQLITE_INCLUDE) \
         -I$(FFMPEG_INCLUDE) \
         -I$(SDL3_MIXER_INCLUDE) \
         -Iinclude/cpp_headers \
         -Iinclude/objc_headers \
         -Isrc/objc \
         -Idatabase

# Library flags
LIB_FLAGS = -L$(SDL3_LIB) -L$(SDL3_IMAGE_LIB) -L$(SDL3_TTF_LIB) \
            -L$(SQLITE_LIB) -L$(FFMPEG_LIB) -L$(SDL3_MIXER_LIB) \
            -lSDL3 -lSDL3_image -lSDL3_ttf -lsqlite3 \
            -lavcodec -lavformat -lavutil -lswscale -lswresample \
            -lSDL3_mixer \
            $(PLATFORM_LIBS) \
            -rpath /usr/local/lib

# Target and sources
TARGET = AtaraxiaSDK
SRC_CPP = src/cpp/main.cpp \
          database/gameScores.cpp \
          database/SDLColors.cpp \
          src/cpp/videoRendering.cpp \
          src/cpp/buttonTasks.cpp

SRC_OBJC = src/objc/cocoaToolbarUI.mm

# Object files
OBJ_CPP = $(SRC_CPP:.cpp=.o)
OBJ_OBJC = $(SRC_OBJC:.mm=.o)
OBJS = $(OBJ_CPP) $(OBJ_OBJC)

# Binary target: build the executable binary
binary: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIB_FLAGS) -o $(TARGET)

all: binary

# Existing build rules
src/cpp/%.o: src/cpp/%.cpp
	$(CXX) $(CXXFLAGS) $(HEADER) -c $< -o $@

database/%.o: database/%.cpp
	$(CXX) $(CXXFLAGS) $(HEADER) -c $< -o $@

src/objc/%.o: src/objc/%.mm
	$(CXX) $(OBJCPPFLAGS) $(HEADER) -c $< -o $@

# Utilities
run: binary
	$(EXPORT_LIB_PATH) && ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf AtaraxiaSDK.app

# Bundle target: creates the .app bundle.
bundle: binary
	@echo "Creating app bundle..."
	@rm -rf AtaraxiaSDK.app
	@mkdir -p AtaraxiaSDK.app/Contents/MacOS
	@mkdir -p AtaraxiaSDK.app/Contents/Frameworks
	@mkdir -p AtaraxiaSDK.app/Contents/Resources
	@cp $(TARGET) AtaraxiaSDK.app/Contents/MacOS/
	@echo "Copying assets to Resources directory..."
	@mkdir -p AtaraxiaSDK.app/Contents/Resources/assets
	@cp -R assets/* AtaraxiaSDK.app/Contents/Resources/assets/
	@echo "Creating Info.plist..."
	@echo '<?xml version="1.0" encoding="UTF-8"?>' > AtaraxiaSDK.app/Contents/Info.plist
	@echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo '<plist version="1.0">' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo '<dict>' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo '    <key>CFBundleExecutable</key>' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo '    <string>AtaraxiaSDK</string>' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo '    <key>CFBundleIdentifier</key>' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo '    <string>com.example.AtaraxiaSDK</string>' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo '    <key>CFBundleName</key>' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo '    <string>AtaraxiaSDK</string>' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo '    <key>CFBundlePackageType</key>' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo '    <string>APPL</string>' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo '</dict>' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo '</plist>' >> AtaraxiaSDK.app/Contents/Info.plist
	@echo "Updating executable rpath and library references..."
	@install_name_tool -add_rpath "@executable_path/../Frameworks" AtaraxiaSDK.app/Contents/MacOS/$(TARGET)
	@install_name_tool -change /usr/local/lib/libSDL3.0.dylib "@executable_path/../Frameworks/libSDL3.0.dylib" AtaraxiaSDK.app/Contents/MacOS/$(TARGET)
	@install_name_tool -change /usr/local/lib/libSDL3_image.0.dylib "@executable_path/../Frameworks/libSDL3_image.0.dylib" AtaraxiaSDK.app/Contents/MacOS/$(TARGET)
	@install_name_tool -change /usr/local/lib/libSDL3_ttf.0.dylib "@executable_path/../Frameworks/libSDL3_ttf.0.dylib" AtaraxiaSDK.app/Contents/MacOS/$(TARGET)
	@install_name_tool -change /usr/local/lib/libsqlite3.dylib "@executable_path/../Frameworks/libsqlite3.dylib" AtaraxiaSDK.app/Contents/MacOS/$(TARGET)
	@install_name_tool -change /usr/local/lib/libavcodec.61.dylib "@executable_path/../Frameworks/libavcodec.61.dylib" AtaraxiaSDK.app/Contents/MacOS/$(TARGET)
	@install_name_tool -change /usr/local/lib/libavformat.61.dylib "@executable_path/../Frameworks/libavformat.61.dylib" AtaraxiaSDK.app/Contents/MacOS/$(TARGET)
	@install_name_tool -change /usr/local/lib/libavutil.59.dylib "@executable_path/../Frameworks/libavutil.59.dylib" AtaraxiaSDK.app/Contents/MacOS/$(TARGET)
	@install_name_tool -change /usr/local/lib/libswscale.8.dylib "@executable_path/../Frameworks/libswscale.8.dylib" AtaraxiaSDK.app/Contents/MacOS/$(TARGET)
	@install_name_tool -change /usr/local/lib/libswresample.5.dylib "@executable_path/../Frameworks/libswresample.5.dylib" AtaraxiaSDK.app/Contents/MacOS/$(TARGET)
	@install_name_tool -change /usr/local/lib/libSDL3_mixer.0.dylib "@executable_path/../Frameworks/libSDL3_mixer.0.dylib" AtaraxiaSDK.app/Contents/MacOS/$(TARGET)
	@echo "Copying required libraries into bundle..."
	@cp $(SDL3_LIB)/libSDL3.0.dylib AtaraxiaSDK.app/Contents/Frameworks/libSDL3.0.dylib
	@cp $(SDL3_IMAGE_LIB)/libSDL3_image.0.dylib AtaraxiaSDK.app/Contents/Frameworks/libSDL3_image.0.dylib
	@cp $(SQLITE_LIB)/libsqlite3.dylib AtaraxiaSDK.app/Contents/Frameworks/libsqlite3.dylib
	@cp $(FFMPEG_LIB)/libavcodec.61.dylib AtaraxiaSDK.app/Contents/Frameworks/libavcodec.61.dylib
	@cp $(FFMPEG_LIB)/libavformat.61.dylib AtaraxiaSDK.app/Contents/Frameworks/libavformat.61.dylib
	@cp $(FFMPEG_LIB)/libavutil.59.dylib AtaraxiaSDK.app/Contents/Frameworks/libavutil.59.dylib
	@cp $(FFMPEG_LIB)/libswscale.8.dylib AtaraxiaSDK.app/Contents/Frameworks/libswscale.8.dylib
	@cp $(FFMPEG_LIB)/libswresample.5.dylib AtaraxiaSDK.app/Contents/Frameworks/libswresample.5.dylib
	@cp $(SDL3_TTF_LIB)/libSDL3_ttf.0.dylib AtaraxiaSDK.app/Contents/Frameworks/libSDL3_ttf.0.dylib
	@cp $(SDL3_MIXER_LIB)/libSDL3_mixer.0.dylib AtaraxiaSDK.app/Contents/Frameworks/libSDL3_mixer.0.dylib
	@echo "Fixing library dependencies inside frameworks..."
	@install_name_tool -id "@executable_path/../Frameworks/libSDL3.0.dylib" AtaraxiaSDK.app/Contents/Frameworks/libSDL3.0.dylib
	@install_name_tool -id "@executable_path/../Frameworks/libSDL3_image.0.dylib" AtaraxiaSDK.app/Contents/Frameworks/libSDL3_image.0.dylib
	@install_name_tool -id "@executable_path/../Frameworks/libSDL3_ttf.0.dylib" AtaraxiaSDK.app/Contents/Frameworks/libSDL3_ttf.0.dylib
	@install_name_tool -id "@executable_path/../Frameworks/libsqlite3.dylib" AtaraxiaSDK.app/Contents/Frameworks/libsqlite3.dylib
	@install_name_tool -id "@executable_path/../Frameworks/libavcodec.61.dylib" AtaraxiaSDK.app/Contents/Frameworks/libavcodec.61.dylib
	@install_name_tool -id "@executable_path/../Frameworks/libavformat.61.dylib" AtaraxiaSDK.app/Contents/Frameworks/libavformat.61.dylib
	@install_name_tool -id "@executable_path/../Frameworks/libavutil.59.dylib" AtaraxiaSDK.app/Contents/Frameworks/libavutil.59.dylib
	@install_name_tool -id "@executable_path/../Frameworks/libswscale.8.dylib" AtaraxiaSDK.app/Contents/Frameworks/libswscale.8.dylib
	@install_name_tool -id "@executable_path/../Frameworks/libswresample.5.dylib" AtaraxiaSDK.app/Contents/Frameworks/libswresample.5.dylib
	@install_name_tool -id "@executable_path/../Frameworks/libSDL3_mixer.0.dylib" AtaraxiaSDK.app/Contents/Frameworks/libSDL3_mixer.0.dylib
	@install_name_tool -change /opt/homebrew/opt/sdl3/lib/libSDL3.0.dylib "@executable_path/../Frameworks/libSDL3.0.dylib" AtaraxiaSDK.app/Contents/Frameworks/libSDL3_image.0.dylib
	@install_name_tool -change /usr/local/lib/libSDL3_ttf.0.dylib "@executable_path/../Frameworks/libSDL3_ttf.0.dylib" AtaraxiaSDK.app/Contents/MacOS/$(TARGET)
	@install_name_tool -change /opt/homebrew/opt/sdl3/lib/libSDL3.0.dylib "@executable_path/../Frameworks/libSDL3.0.dylib" AtaraxiaSDK.app/Contents/Frameworks/libSDL3_mixer.0.dylib
	@echo "Copying database to Resources directory..."
	@cp -f scoresDatabase.db AtaraxiaSDK.app/Contents/Resources/ || true

# Sign target: signs the .app bundle.
sign: bundle
	@echo "Signing app bundle..."
	@codesign -s - --deep --force --verbose AtaraxiaSDK.app

# App target: builds, bundles, and signs the app.
# This target is phony so that typing "make AtaraxiaSDK" will build the complete app package.
.PHONY: AtaraxiaSDK
AtaraxiaSDK: sign
	@echo "App package created: AtaraxiaSDK.app"

# Autodetect platform and run the corresponding build script
platform-build:
ifeq ($(UNAME_S), Darwin)
	$(info Running macOS build script)
	$(shell ./build.sh)
else ifeq ($(UNAME_S), Linux)
	$(info Running Linux build script)
	$(shell ./build.sh)
else ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
	$(info Running Windows build script)
	$(shell build.bat)
else
	$(error Unsupported platform)
endif

.PHONY: all clean run platform-build
