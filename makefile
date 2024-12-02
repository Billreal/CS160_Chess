
# Chess: src/main.o
# 	g++ src/main.o -o chess -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2
# src/main.o: src/main.cpp
# 	g++ src/main.cpp -o src/main.o -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2

LINKER = -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2
COMPILER_FLAG= -std=c++20 -lmingw32 

# Target to clean up object files and initialize Chess.exe
all: clean chess compile
	./chess
# Target to clean up object files
clean:
	@powershell -Command "Get-ChildItem -Path src -Filter *.o | Remove-Item -Force"
	@powershell -Command "Get-ChildItem -Path src -Filter chess.exe | Remove-Item -Force"

# clean:
# 	rm -f $(wildcard src/*.o)
# 	rm -f chess.exe

# chess: main color board colorScheme background pieces coordinate
compile:
	g++ -o chess $(wildcard src/*.o) $(COMPILER_FLAG) $(LINKER)
chess: main color board colorScheme coordinate pieces button

main:
	g++ -c src/main.cpp -o src/main.o $(COMPILER_FLAG) $(LINKER)

board:
	g++ -c src/board.cpp -o src/board.o $(COMPILER_FLAG) $(LINKER)

color: 
	g++ -c src/color.cpp -o src/color.o $(COMPILER_FLAG) $(LINKER)

colorScheme:
	g++ -c src/colorScheme.cpp -o src/colorScheme.o $(COMPILER_FLAG) $(LINKER)

# background:
# 	g++ -c src/background.cpp -o src/background.o $(COMPILER_FLAG) $(LINKER)

pieces:
	g++ -c src/pieces.cpp -o src/pieces.o $(COMPILER_FLAG) $(LINKER)

coordinate:
	g++ -c src/coordinate.cpp -o src/coordinate.o $(COMPILER_FLAG) $(LINKER)

button:
	g++ -c src/button.cpp -o src/button.o $(COMPILER_FLAG) $(LINKER)

communicator:
	g++ -c src/communicator.cpp -o src/communicator.o $(COMPILER_FLAG) $(LINKER)
# testing: clean communicator board button coordinate pieces colorScheme color 
# 	g++ -c testing/communicatorTest.cpp -o testing/communicatorTest.o $(COMPILER_FLAG) $(LINKER)
# 	g++ -o communicatorTest testing/communicatorTest.o src/button.o src/coordinate.o src/pieces.o src/colorScheme.o src/color.o src/board.o src/communicator.o $(COMPILER_FLAG) $(LINKER)
# 	./communicatorTest.exe

testing: color board colorScheme coordinate pieces button communicator
	g++ -c testing/communicatorTest.cpp -o testing/communicatorTest.o $(COMPILER_FLAG) $(LINKER)
	g++ -o communicatorTest testing/communicatorTest.o src/communicator.o src/board.o src/color.o src/colorScheme.o src/coordinate.o src/pieces.o src/button.o $(COMPILER_FLAG) $(LINKER) 
	./communicatorTest.exe