PKG_CONFIG_LIBS := sdl2 SDL2_ttf SDL2_image
CXXFLAGS := `pkg-config --cflags $(PKG_CONFIG_LIBS)`
LDLIBS := -lm `pkg-config --libs $(PKG_CONFIG_LIBS)`

LDFLAGS += -Wl,-rpath,$(shell pkg-config --variable=libdir sdl2)

CXXFLAGS += -g3 -fsanitize=address
LDFLAGS += -fsanitize=address

SRCS := src/board.cpp src/game.cpp src/timer.cpp src/draw.cpp src/ui.cpp src/options.cpp src/events.cpp
OBJS := $(SRCS:.cpp=.o)

TARGET := minesweeper

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(LDLIBS) -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET)
	$(RM) $(TARGET) $(OBJS)
