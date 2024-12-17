# Detect the platform
UNAME_S := $(shell uname -s)

# Compiler and linker flags
COMPILER_FLAG_WINDOWS = -std=c++20 -lmingw32 
COMPILER_FLAG_MACOS = -std=c++20 -Wall
LINKER_FLAGS_WINDOWS = -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Lsrc/SDL2/lib -Lsrc/SDL2/bin -Isrc/SDL2/include -Isrc/SDL2/include/SDL2
LINKER_FLAGS_MACOS = -framework SDL2 -framework SDL2_ttf -framework SDL2_image -framework SDL2_mixer -rpath /Library/Frameworks -F/Library/Frameworks -I/Library/Frameworks/SDL2_ttf.framework/Headers -I/Library/Frameworks/SDL2_image.framework/Headers -I/Library/Frameworks/SDL2_mixer.framework/Headers

# Platform-specific settings
ifeq ($(UNAME_S), Linux)
	COMPILER_FLAG = $(COMPILER_FLAG_MACOS)
    LINKER_FLAGS = $(LINKER_FLAGS_MACOS)
    CLEAN_CMD = rm -rf $(wildcard src/*.o) | rm -rf chess.exe
else ifeq ($(UNAME_S), Darwin)
	COMPILER_FLAG = $(COMPILER_FLAG_MACOS)
	LINKER_FLAGS = $(LINKER_FLAGS_MACOS)
	CLEAN_CMD = rm -rf $(wildcard src/*.o) | rm -rf chess.exe
else ifeq ($(OS), Windows_NT)
	COMPILER_FLAG = $(COMPILER_FLAG_WINDOWS)
    LINKER_FLAGS = $(LINKER_FLAGS_WINDOWS)
    CLEAN_CMD = @powershell -Command "Get-ChildItem -Path src -Filter *.o | Remove-Item -Force | Get-ChildItem -Path src -Filter chess.exe | Remove-Item -Force"
endif

# Target to clean up object files and initialize Chess.exe
all: clean chess compile
	./chess

# Target to clean up object files
clean: 
	$(CLEAN_CMD)

# Compile and link the executable
compile: 
	g++ -o chess $(wildcard src/*.o) $(COMPILER_FLAG) $(LINKER_FLAGS)

chess: main color board colorScheme coordinate pieces button communicator gameStateManager popup sound music
	
# Individual compilation targets
main: 
	g++ $(COMPILER_FLAG) -c src/main.cpp -o src/main.o $(LINKER_FLAGS)

board: 
	g++ -c src/board.cpp -o src/board.o $(COMPILER_FLAG) $(LINKER_FLAGS)

color: 
	g++ -c src/color.cpp -o src/color.o $(COMPILER_FLAG) $(LINKER_FLAGS)

colorScheme:
	g++ -c src/colorScheme.cpp -o src/colorScheme.o $(COMPILER_FLAG) $(LINKER_FLAGS)

pieces:
	g++ -c src/pieces.cpp -o src/pieces.o $(COMPILER_FLAG) $(LINKER_FLAGS)

coordinate:
	g++ -c src/coordinate.cpp -o src/coordinate.o $(COMPILER_FLAG) $(LINKER_FLAGS)

button:
	g++ -c src/button.cpp -o src/button.o $(COMPILER_FLAG) $(LINKER_FLAGS)

communicator:
	g++ -c src/communicator.cpp -o src/communicator.o $(COMPILER_FLAG) $(LINKER_FLAGS)

gameStateManager:
	g++ -c src/gameStateManager.cpp -o src/gameStateManager.o $(COMPILER_FLAG) $(LINKER_FLAGS)

popup:
	g++ -c src/popup.cpp -o src/popup.o $(COMPILER_FLAG) $(LINKER_FLAGS)

sound:
	g++ -c src/sound.cpp -o src/sound.o $(COMPILER_FLAG) $(LINKER_FLAGS)

music:
	g++ -c src/music.cpp -o src/music.o $(COMPILER_FLAG) $(LINKER_FLAGS)

test:
	g++ -c testing/communicatorTest.cpp -o testing/communicatorTest.o $(COMPILER_FLAG) $(LINKER_FLAGS)
	g++ -o testing.exe testing/communicatorTest.o $(COMPILER_FLAG) $(LINKER_FLAGS)
# CXX = g++

# CXXFLAGS = -Wall -F /Library/Frameworks
# LDFLAGS = -framework SDL2 -F /Library/Frameworks -I /Library/Frameworks/SDL2.framework/Headers

# all: main

# main: src/main.o
# 	$(CXX) src/main.o -o main $(LDFLAGS)

# obj/main.o : src/main.cpp
# 	$(CXX) $(CXXFLAGS) -c src/main.cpp -o src/main.o

# clean:
# 	rm src/main.o main