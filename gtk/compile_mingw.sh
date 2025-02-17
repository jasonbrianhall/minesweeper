#!/bin/bash
x86_64-w64-mingw32-gcc -o main.exe gtk_minesweeper.cpp highscores.cpp `mingw64-pkg-config --cflags gtk+-3.0 --libs gtk+-3.0` -lstdc++
