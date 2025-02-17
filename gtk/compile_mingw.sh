#!/bin/bash
# Theoretically compiles for Windows but has so many DLL requirements that the distributed packaged is massive and unwieldy
x86_64-w64-mingw32-gcc -o minesweeper_gtk.exe gtk_minesweeper.cpp highscores.cpp `mingw64-pkg-config --cflags gtk+-3.0 --libs gtk+-3.0` -lstdc++
