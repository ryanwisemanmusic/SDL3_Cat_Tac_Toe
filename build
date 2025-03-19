#!/bin/bash

# Compiler
CXX="clang++"

# Compiler flags
CXXFLAGS="-std=c++17 -Wall -Wextra -O2"
OBJCPPFLAGS="$CXXFLAGS"

# Detect OS
UNAME_S=$(uname -s)

if [ "$UNAME_S" = "Darwin" ]; then
    # macOS paths
    SDL3_INCLUDE="/opt/homebrew/Cellar/sdl3/3.2.8/include"
    SDL3_LIB="/opt/homebrew/Cellar/sdl3/3.2.8/lib"
    SDL3_IMAGE_INCLUDE="/opt/homebrew/Cellar/sdl3_image/3.2.4/include"
    SDL3_IMAGE_LIB="/opt/homebrew/Cellar/sdl3_image/3.2.4/lib"
    SDL3_TTF_INCLUDE="/usr/local/include/SDL3_ttf"
    SDL3_TTF_LIB="/usr/local/lib"
    SQLITE_INCLUDE="/opt/homebrew/Cellar/sqlite/3.49.1/include"
    SQLITE_LIB="/opt/homebrew/Cellar/sqlite/3.49.1/lib"

    # macOS-specific libraries
    PLATFORM_LIBS="-framework Cocoa -framework OpenGL -lobjc"

    # macOS-specific export path
    EXPORT_LIB_PATH="export DYLD_LIBRARY_PATH=\"/usr/local/lib:\$DYLD_LIBRARY_PATH\""
else
    # Linux paths
    SDL3_INCLUDE="/usr/include/SDL3"
    SDL3_LIB="/usr/lib"
    SDL3_IMAGE_INCLUDE="/usr/include/SDL3_image"
    SDL3_IMAGE_LIB="/usr/lib"
    SDL3_TTF_INCLUDE="/usr/include/SDL3_ttf"
    SDL3_TTF_LIB="/usr/lib"
    SQLITE_INCLUDE="/usr/include"
    SQLITE_LIB="/usr/lib"

    # Linux-specific libraries
    PLATFORM_LIBS="-lGL -ldl -lpthread -lm"

    # Linux-specific export path
    EXPORT_LIB_PATH="export LD_LIBRARY_PATH=\"/usr/lib:\$LD_LIBRARY_PATH\""
fi

# Header directories
HEADER="-isystem $SDL3_INCLUDE \
        -I$SDL3_IMAGE_INCLUDE \
        -I$SDL3_TTF_INCLUDE \
        -Iinclude/cpp_headers \
        -Iinclude/objc_headers \
        -Isrc/objc \
        -Idatabase \
        -I$SQLITE_INCLUDE"

# Library flags
LIB_FLAGS="-L$SDL3_LIB -L$SDL3_IMAGE_LIB -L$SDL3_TTF_LIB \
           -L$SQLITE_LIB \
           -lSDL3 -lSDL3_image -lSDL3_ttf -lsqlite3 \
           $PLATFORM_LIBS \
           -rpath /usr/local/lib"

# Target and sources
TARGET="AtaraxiaSDK"
SRC_CPP="src/cpp/main.cpp database/gameScores.cpp database/SDLColors.cpp"
SRC_OBJC="src/objc/cocoaToolbarUI.mm"

# Object files
OBJ_CPP=$(echo $SRC_CPP | tr ' ' '\n' | sed 's/.cpp/.o/g')
OBJ_OBJC=$(echo $SRC_OBJC | tr ' ' '\n' | sed 's/.mm/.o/g')
OBJS="$OBJ_CPP $OBJ_OBJC"

# Build rules
build_target() {
    $CXX $CXXFLAGS $OBJS $LIB_FLAGS -o $TARGET
}

# Compile .cpp files
compile_cpp() {
    for src in $SRC_CPP; do
        obj="${src/.cpp/.o}"
        $CXX $CXXFLAGS $HEADER -c $src -o $obj
    done
}

# Compile .mm files
compile_objc() {
    for src in $SRC_OBJC; do
        obj="${src/.mm/.o}"
        $CXX $OBJCPPFLAGS $HEADER -c $src -o $obj
    done
}

# Utilities
run() {
    $EXPORT_LIB_PATH && ./$TARGET
}

clean() {
    rm -f $OBJS $TARGET
}

# Main
case "$1" in
    "build")
        compile_cpp
        compile_objc
        build_target
        ;;
    "run")
        run
        ;;
    "clean")
        clean
        ;;
    *)
        echo "Usage: $0 {build|run|clean}"
        exit 1
        ;;
esac
