CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lncurses
TARGET = minesweeper
SRCS = minesweeper.cpp highscores.cpp
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link the final executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

# Install target (optional)
install: $(TARGET)
	install -d $(DESTDIR)/usr/local/bin/
	install -m 755 $(TARGET) $(DESTDIR)/usr/local/bin/

# Uninstall target (optional)
uninstall:
	rm -f $(DESTDIR)/usr/local/bin/$(TARGET)

# Check if required tools are available
check-tools:
	@which $(CXX) > /dev/null || (echo "Error: g++ not found"; exit 1)
	@echo "All required tools are available"

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build the minesweeper game (default)"
	@echo "  clean      - Remove build files"
	@echo "  install    - Install the game to /usr/local/bin"
	@echo "  uninstall  - Remove the installed game"
	@echo "  check-tools- Verify required build tools are available"

.PHONY: all clean install uninstall check-tools help
