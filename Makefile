SRCS = src/board.cpp src/game.cpp src/timer.cpp src/draw.cpp src/ui.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = game
DEBUG_FLAGS = -fsanitize=address -g

all: $(OBJS)
	g++ $(OBJS) -o $(TARGET) -I/usr/include/SDL2 -lSDL2 -lSDL2_ttf -lSDL2_image

debug: LDFLAGS += $(DEBUG_FLAGS)
debug: $(OBJS)
	g++ $(OBJS) $(LDFLAGS) -o $(TARGET) -I/usr/include/SDL2 -lSDL2 -lSDL2_ttf -lSDL2_image

clean:
	$(RM) $(OBJS) $(TARGET)
