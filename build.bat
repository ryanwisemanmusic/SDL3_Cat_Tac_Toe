@echo off
setlocal

:: Compiler and flags
set CXX=clang++
set CXXFLAGS=-std=c++17 -Wall -Wextra -O2
set OBJCPPFLAGS=%CXXFLAGS%

:: Detect OS
set UNAME_S=%OS%

if "%UNAME_S%"=="Darwin" (
    :: macOS paths
    set SDL3_INCLUDE=/opt/homebrew/Cellar/sdl3/3.2.8/include
    set SDL3_LIB=/opt/homebrew/Cellar/sdl3/3.2.8/lib
    set SDL3_IMAGE_INCLUDE=/opt/homebrew/Cellar/sdl3_image/3.2.4/include
    set SDL3_IMAGE_LIB=/opt/homebrew/Cellar/sdl3_image/3.2.4/lib
    set SDL3_TTF_INCLUDE=/usr/local/include/SDL3_ttf
    set SDL3_TTF_LIB=/usr/local/lib
    set SQLITE_INCLUDE=/opt/homebrew/Cellar/sqlite/3.49.1/include
    set SQLITE_LIB=/opt/homebrew/Cellar/sqlite/3.49.1/lib

    :: macOS-specific libraries
    set PLATFORM_LIBS=-framework Cocoa -framework OpenGL -lobjc

    :: macOS-specific export path
    set EXPORT_LIB_PATH=export DYLD_LIBRARY_PATH="/usr/local/lib:$DYLD_LIBRARY_PATH"
) else if "%UNAME_S%"=="Linux" (
    :: Linux paths
    set SDL3_INCLUDE=/usr/include/SDL3
    set SDL3_LIB=/usr/lib
    set SDL3_IMAGE_INCLUDE=/usr/include/SDL3_image
    set SDL3_IMAGE_LIB=/usr/lib
    set SDL3_TTF_INCLUDE=/usr/include/SDL3_ttf
    set SDL3_TTF_LIB=/usr/lib
    set SQLITE_INCLUDE=/usr/include
    set SQLITE_LIB=/usr/lib

    :: Linux-specific libraries
    set PLATFORM_LIBS=-lGL -ldl -lpthread -lm

    :: Linux-specific export path
    set EXPORT_LIB_PATH=export LD_LIBRARY_PATH="/usr/lib:$LD_LIBRARY_PATH"
) else (
    :: Windows paths (MinGW or similar toolchain)
    set SDL3_INCLUDE=C:/SDL3/include
    set SDL3_LIB=C:/SDL3/lib
    set SDL3_IMAGE_INCLUDE=C:/SDL3_image/include
    set SDL3_IMAGE_LIB=C:/SDL3_image/lib
    set SDL3_TTF_INCLUDE=C:/SDL3_ttf/include
    set SDL3_TTF_LIB=C:/SDL3_ttf/lib
    set SQLITE_INCLUDE=C:/sqlite/include
    set SQLITE_LIB=C:/sqlite/lib

    :: Windows-specific libraries
    set PLATFORM_LIBS=-lSDL3 -lSDL3_image -lSDL3_ttf -lsqlite3 -lGL -lmingw32 -lSDL3main

    :: Windows-specific export path (MinGW uses the same approach)
    set EXPORT_LIB_PATH=set LIBRARY_PATH=%LIBRARY_PATH%;C:/SDL3/lib;C:/SDL3_image/lib;C:/SDL3_ttf/lib;C:/sqlite/lib
)

:: Header directories
set HEADER=-isystem %SDL3_INCLUDE% -I%SDL3_IMAGE_INCLUDE% -I%SDL3_TTF_INCLUDE% -Iinclude/cpp_headers -Iinclude/objc_headers -Isrc/objc -Idatabase -I%SQLITE_INCLUDE%

:: Library flags
set LIB_FLAGS=-L%SDL3_LIB% -L%SDL3_IMAGE_LIB% -L%SDL3_TTF_LIB% -L%SQLITE_LIB% -lSDL3 -lSDL3_image -lSDL3_ttf -lsqlite3 %PLATFORM_LIBS%

:: Target and sources
set TARGET=AtaraxiaSDK
set SRC_CPP=src/cpp/main.cpp database/gameScores.cpp database/SDLColors.cpp
set SRC_OBJC=src/objc/cocoaToolbarUI.mm

:: Object files
set OBJ_CPP=%SRC_CPP:.cpp=.o%
set OBJ_OBJC=%SRC_OBJC:.mm=.o%
set OBJS=%OBJ_CPP% %OBJ_OBJC%

:: Build rules
echo Building %TARGET%
%CXX% %CXXFLAGS% %OBJS% %LIB_FLAGS% -o %TARGET%

:: Run the built application (for testing)
echo Running %TARGET%
%EXPORT_LIB_PATH% && ./%TARGET%

:: Clean up object files
echo Cleaning up object files...
del %OBJS%

endlocal
