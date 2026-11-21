PKG_CONFIG_LIBS := sdl2 SDL2_ttf SDL2_image
CXXFLAGS := `pkg-config --cflags $(PKG_CONFIG_LIBS)`
LDLIBS := -lm `pkg-config --libs $(PKG_CONFIG_LIBS)`

LDFLAGS += -Wl,-rpath,$(shell pkg-config --variable=libdir sdl2)

CXXFLAGS += -g3 -fsanitize=address
LDFLAGS += -fsanitize=address

SRCS := src/board.cpp src/timer.cpp src/draw.cpp src/ui.cpp src/options.cpp src/events.cpp src/cell.cpp src/solver/solver.cpp
TARGET_SRCS := $(SRCS) src/main.cpp

OBJS := $(SRCS:.cpp=.o)
TARGET_OBJS := $(TARGET_SRCS:.cpp=.o)

TEST_SRCS := tests/solver.cpp
TEST_OBJS := $(TEST_SRCS:.cpp=.o)

TARGET := minesweeper

all: $(TARGET)

$(TARGET): $(TARGET_OBJS) $(OBJS)
	$(CXX) $(LDFLAGS) $(LDLIBS) -o $@ $^

check: $(TEST_OBJS) $(OBJS)
	$(CXX) $(LDFLAGS) $(LDLIBS) -lcriterion -o $@ $^
	
clean:
	$(RM) $(TARGET) check $(OBJS) $(TARGET_OBJS) $(TEST_OBJS)
