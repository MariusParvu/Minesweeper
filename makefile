OBJS = mines.o canvas.o game.o
LIB = -L/opt/homebrew/lib/
FLAGS = -lSDL2 -lSDL2_image
INCLUDE = -I/opt/homebrew/include/SDL2

all 	: $(OBJS)
	arch -arm64 clang++ -std=c++11 main.cpp $(OBJS) $(LIB) $(FLAGS) $(INCLUDE) -o mines

game.o 	: game.cpp canvas.o mines.o
	arch -arm64 clang++ -std=c++11 -c $< -o $@ $(INCLUDE)

%.o 	: %.cpp %.h
	arch -arm64 clang++ -std=c++11 -c $< -o $@ $(INCLUDE)

clean	:
	rm *.o
	rm mines
