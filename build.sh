g++ -std=c++17 src/*.cpp -I./src/ -Os $(pkg-config --cflags --libs sdl2 SDL2_image SDL2_mixer) -o mesopotamia
echo "Linux built"
