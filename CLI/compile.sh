#!/bin/bash

# Check and compile Linux version if g++ is available
if command -v g++ &> /dev/null; then
    echo "Compiling Linux version..."
    g++ minesweeper.cpp highscores.cpp -lncurses -o minesweeper
else
    echo "g++ not found - skipping Linux build"
fi

# Check and compile Windows version if cross-compiler is available
if command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    mkdir win_minesweeper -p

    echo "Compiling Windows version (PDCurses) ..."
    x86_64-w64-mingw32-g++ minesweeper.cpp highscores.cpp -lpdcurses -std=c++14 -o win_minesweeper/minesweepr_pdcurses.exe
    ./collect_dlls.sh win_minesweeper/minesweepr_pdcurses.exe /usr/x86_64-w64-mingw32/sys-root/mingw/bin win_minesweeper

else
    echo "Windows cross-compiler not found - skipping Windows build"
fi

