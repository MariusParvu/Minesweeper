OBJS = mines.o canvas.o game.o

all 	: 	$(OBJS)
	arch -x86_64 clang++ -std=c++11 main.cpp $(OBJS) -L/usr/local/homebrew/lib/ -lSDL2 -lSDL2_image -I/usr/local/homebrew/include/SDL2 -o mines

%.o 	: %.cpp %.h
	arch -x86_64 clang++ -std=c++11 -c $< -o $@ -L/usr/local/homebrew/lib/ -lSDL2 -lSDL2_image -I/usr/local/homebrew/include/SDL2

clean	:
	rm *.o
	rm mines
