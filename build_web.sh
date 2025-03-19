if ! command -v emcc &> /dev/null; then
    echo "ERROR: emcc command not found. Please install and activate Emscripten first."
    echo "Visit https://emscripten.org/docs/getting_started/downloads.html for installation instructions."
    exit 1
fi

echo "DEBUG: Installing required ports..."
embuilder build sdl3 sqlite3

echo "DEBUG: Starting web build using Emscripten..."
echo "DEBUG: Compiling source files and embedding asset folders..."

emcc \
  src/cpp/main.cpp \
  database/gameScores.cpp \
  database/SDLColors.cpp \
  src/cpp/videoRendering.cpp \
  src/cpp/buttonTasks.cpp \
  src/objc/cocoaToolbarUI.mm \
  -O2 \
  -s WASM=1 \
  -s USE_SDL=3 \
  -s USE_SQLITE3=1 \
  -DWEB_BUILD \
  -DNO_FFMPEG \
  -I/opt/homebrew/Cellar/sdl3/3.2.8/include \
  -I/opt/homebrew/Cellar/sdl3_image/3.2.4/include \
  -I/usr/local/include/SDL3_ttf \
  -I/opt/homebrew/Cellar/sqlite/3.49.1/include \
  -Idatabase \
  -Isrc/cpp \
  -Isrc/objc \
  --preload-file assets \
  --preload-file fonts \
  --preload-file images \
  --preload-file video \
  --preload-file database/scoresDatabase.db@database/scoresDatabase.db \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s ASSERTIONS=1 \
  -s DEMANGLE_SUPPORT=1 \
  -o index.html

if [ $? -eq 0 ]; then
    echo "DEBUG: Web build completed successfully."
    echo "DEBUG: Generated index.html (and corresponding .js and .wasm files) are ready for deployment to itch.io."
else
    echo "DEBUG: Web build encountered errors."
fi
