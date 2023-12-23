OBJS = mines.o canvas.o game.o
LIB = -L/usr/local/homebrew/lib/
FLAGS = -lSDL2 -lSDL2_image
INCLUDE = -I/usr/local/homebrew/include/SDL2

all 	: $(OBJS)
	arch -x86_64 clang++ -std=c++11 main.cpp $(OBJS) $(LIB) $(FLAGS) $(INCLUDE) -o mines

game.o 	: game.cpp canvas.o mines.o
	arch -x86_64 clang++ -std=c++11 -c $< -o $@ $(INCLUDE)

%.o 	: %.cpp %.h
	arch -x86_64 clang++ -std=c++11 -c $< -o $@ $(INCLUDE)

clean	:
	rm *.o
	rm mines
