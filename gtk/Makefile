CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0)

SRCS = gtk_minesweeper.cpp gtk_utilities.cpp highscores.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = minesweeper

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
