
# Chess: src/main.o
# 	g++ src/main.o -o chess -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2
# src/main.o: src/main.cpp
# 	g++ src/main.cpp -o src/main.o -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2
COMPILER_FLAG= -std=c++20 -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2 -lmingw32 -lSDL2main -lSDL2 -Lsrc/SDL2/lib
all: chess
	rm -f $(wildcard src/*.o)

clean:
	rm -f $(wildcard src/*.o)
	rm -f chess.exe

chess: src/main.o src/board.o src/color.o src/colorScheme.o
	g++ -o chess $(wildcard src/*.o) $(COMPILER_FLAG)

src/main.o: src/main.cpp
	g++ -c src/main.cpp -o src/main.o $(COMPILER_FLAG)

src/board.o: src/board.cpp
	g++ -c src/board.cpp -o src/board.o $(COMPILER_FLAG)

src/color.o: src/color.cpp
	g++ -c src/color.cpp -o src/color.o $(COMPILER_FLAG)

src/colorScheme.o: src/colorScheme.cpp
	g++ -c src/colorScheme.cpp -o src/colorScheme.o $(COMPILER_FLAG)

