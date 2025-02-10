#!/bin/bash

# Check and compile Linux version if g++ is available
if command -v g++ &> /dev/null; then
    echo "Compiling Linux version..."
    g++ minesweeper.cpp -lncurses -o minesweeper
else
    echo "g++ not found - skipping Linux build"
fi
