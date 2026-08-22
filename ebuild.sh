source ~/Emscripten/emsdk/emsdk_env.sh # might cause issues if somebody else tries to compile it for web

emcc src/*.cpp -o index.html \
    -std=c++17 \
    -I./src \
    -Os \
    --shell-file src/shell.html \
    -s USE_SDL=2 \
    -s USE_SDL_IMAGE=2 \
    -s USE_SDL_MIXER=2 \
    -s SDL2_IMAGE_FORMATS='["png"]' \
    -s SDL2_MIXER_FORMATS='["mp3"]' \
    --preload-file assets@assets \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s EXIT_RUNTIME=1
echo "Enscripten build"
