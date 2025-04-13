# DOS Minesweeper

## Overview

This is a classic Minesweeper game implemented in C++ for MS-DOS/DJGPP, featuring a ncurses-like interface and multiple difficulty levels. The game provides a nostalgic gaming experience with support for custom board sizes and high score tracking.

## Features

- 3 Preset Difficulty Levels:
  - Easy: 9x9 grid with 10 mines
  - Medium: 16x16 grid with 40 mines
  - Hard: 16x30 grid with 99 mines
- Custom Board Configuration
- Seed-based Random Number Generation
- High Score Tracking
- Colorful Terminal Interface

## Game Controls

- **Arrow Keys**: Move cursor
- **Space**: Reveal cell
- **F**: Flag/unflag cell
- **H**: Show/hide help
- **C**: Clear screen
- **N**: New game
- **R**: Reset current game
- **K**: View high scores
- **Q**: Quit game

## High Score System

- Saves top 10 scores per difficulty level
- Stores scores in `~/.minesweeper/scores.txt` (Unix-like systems) or `%APPDATA%\.minesweeper\scores.txt` (Windows)

## Build Requirements

- DJGPP Compiler
- ncurses-compatible library (included in project)
- DOS/DOSBox environment

## Build Instructions

### Prerequisites

- Docker
- Make
- wget
- DOSBox (optional, for running)

### Compilation

The project uses a Makefile with Docker to simplify cross-platform compilation:

```bash
# Build the MS-DOS executable
make msdos

# Build and run in DOSBox
make run

# Clean generated files
make clean
```

#### Build Process Details

1. Pulls a DJGPP Docker image
2. Downloads CSDPMI (DOS extender)
3. Compiles the project using g++ in the Docker environment
4. Generates `mnsweep.exe`

### Manual Compilation (Alternative)

If you prefer not using Docker:

```bash
# Using DJGPP compiler directly
g++ minesweeper.cpp highscores.cpp ncurses.cpp -o mnsweep.exe
```

## Project Structure

- `minesweeper.cpp`: Main game logic
- `highscores.cpp`: High score management
- `highscores.h`: High score class definition
- `ncurses.cpp`: DOS-compatible ncurses-like implementation
- `ncurses.h`: Header for ncurses compatibility
- `thread.h`: Minimal thread compatibility for DOS

## Special Notes

- Designed for MS-DOS/DJGPP environment
- Uses a custom ncurses-like implementation
- Cross-platform path handling for high score storage
- Seed-based random number generation allows reproducible game layouts

## License

MIT License

Copyright (c) 2025 Jason Brian Hall

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Disclaimer

This is a classic implementation of Minesweeper for educational and nostalgic purposes, targeting the MS-DOS platform.
