x86_64-w64-mingw32-g++ -std=c++17 src/*.cpp src/resource.res \
    -I$HOME/win-libs/SDL2/x86_64-w64-mingw32/include \
    -I./src \
    -L$HOME/win-libs/SDL2/x86_64-w64-mingw32/lib \
    -lmingw32 \
    -lSDL2_image \
    -lSDL2_mixer \
    -lSDL2main \
    -Os \
    -lSDL2 \
    -static-libgcc -static-libstdc++ \
    -Wl,--subsystem,windows \
    -o mesopotamia.exe
echo "Windows built"
