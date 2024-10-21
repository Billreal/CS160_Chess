
# Chess: src/main.o
# 	g++ src/main.o -o chess -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2
# src/main.o: src/main.cpp
# 	g++ src/main.cpp -o src/main.o -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2
LINKER = -lSDL2main -lSDL2 -lSDL2_image -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2       
COMPILER_FLAG= -std=c++20 -lmingw32 
all: clean chess
	rm -f $(wildcard src/*.o)
	./chess
clean:
	rm -f $(wildcard src/*.o)
	rm -f chess.exe

chess: main color board colorScheme background
	g++ -o chess $(wildcard src/*.o) $(COMPILER_FLAG) $(LINKER)

main:
	g++ -c src/main.cpp -o src/main.o $(COMPILER_FLAG) $(LINKER)

board:
	g++ -c src/board.cpp -o src/board.o $(COMPILER_FLAG) $(LINKER)

color: 
	g++ -c src/color.cpp -o src/color.o $(COMPILER_FLAG) $(LINKER)

colorScheme:
	g++ -c src/colorScheme.cpp -o src/colorScheme.o $(COMPILER_FLAG) $(LINKER)

background:
	g++ -c src/background.cpp -o src/background.o $(COMPILER_FLAG) $(LINKER)