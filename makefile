
# Chess: src/main.o
# 	g++ src/main.o -o chess -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2
# src/main.o: src/main.cpp
# 	g++ src/main.cpp -o src/main.o -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2
all: chess

chess: src/main.o
	g++ -o chess src/main.o -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2
	
src/main.o: src/main.cpp
	g++ -c src/main.cpp -o src/main.o -Lsrc/SDL2/bin -Isrc/SDL2/include/SDL2

