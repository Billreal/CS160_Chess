Get-ChildItem -Path src -Filter *.o | Remove-Item -Force | Get-ChildItem -Path src -Filter chess.exe | Remove-Item -Force
g++ -c src/main.cpp -o src/main.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
g++ -c src/color.cpp -o src/color.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
g++ -c src/board.cpp -o src/board.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
g++ -c src/colorScheme.cpp -o src/colorScheme.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
g++ -c src/coordinate.cpp -o src/coordinate.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
g++ -c src/pieces.cpp -o src/pieces.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
g++ -c src/button.cpp -o src/button.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
g++ -c src/communicator.cpp -o src/communicator.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
g++ -c src/gameStateManager.cpp -o src/gameStateManager.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
g++ -c src/popup.cpp -o src/popup.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
g++ -c src/sound.cpp -o src/sound.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
g++ -c src/music.cpp -o src/music.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
g++ -o chess src/board.o src/button.o src/color.o src/colorScheme.o src/communicator.o src/coordinate.o src/gameStateManager.o src/main.o src/music.o src/pieces.o src/popup.o src/sound.o -std=c++20 -lmingw32  -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
./chess