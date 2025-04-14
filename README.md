# 🎮 Minesweeper

A modern, feature-rich implementation of the classic Minesweeper game, available in Windows Forms, Linux GTK, and Terminal versions.

## 📦 Versions

### Windows Forms Version
A polished, graphical implementation built with Windows Forms and C++, featuring a modern user interface.

### Linux GTK Version
A polished, graphical implementation built with GTK and C++, featuring a modern user interface.

### Terminal Version
A classic, ncurses-based implementation perfect for terminal environments and systems without graphical interfaces.

Versions include:

* Linux
* Windows
* MS-DOS

For MS-DOS, their are two different versions.  In the msdos folder, their is a fake ncurses that uses conio.h for movement.  In the CLI foloder, their is a ncurses/pdcurses version.  Both are very similar and functionally the same.

## ✨ Common Features

All versions include:
- Four difficulty levels:
  - Easy (9x9 grid, 10 mines)
  - Medium (16x16 grid, 40 mines)
  - Hard (16x30 grid, 99 mines)
  - Custom board size with custom number of mines
- High score system with persistent storage
  - Separate leaderboards for each difficulty
  - Top 10 times per difficulty level
  - Scores saved in platform-appropriate location:
    - Windows: %APPDATA%\.minesweeper\scores.txt
    - Unix: ~/.minesweeper/scores.txt
- Color-coded numbers for adjacent mine counts
- Game timer with MM:SS format display
- First-click protection (never hit a mine on first move)
- Quick-reveal for completed number tiles

## 🎯 Controls

### Windows Forms Version
Mouse Controls:
- **Left Click**: Reveal cell
- **Right Click**: Flag/unflag cell
- **Left Click on Number**: Quick-reveal adjacent cells (if correct number of flags placed)

Keyboard Shortcuts:
- **N**: Start new game
- **F1**: Switch to Easy difficulty
- **F2**: Switch to Medium difficulty
- **F3**: Switch to Hard difficulty
- **Ctrl+H**: View high scores

### GTK Version
Mouse Controls:
- **Left Click**: Reveal cell
- **Right Click**: Flag/unflag cell
- **Left Click on Number**: Quick-reveal adjacent cells (if correct number of flags placed)

Keyboard Shortcuts:
- **CTRL+N**: Start new game
- **CTRL+1**: Switch to Easy difficulty
- **CTRL+2**: Switch to Medium difficulty
- **CTRL+3**: Switch to Hard difficulty
- **Ctrl+H**: View high scores
- **Ctrl+Q**: Quit
- **Ctrl+R**: Reset Game

### Terminal Version
- **Arrow Keys**: Move cursor
- **Space**: Reveal cell
- **F**: Flag/unflag cell
- **H**: Show/hide help menu
- **K**: View high scores
- **N**: Start new game
- **R**: Restart current game
- **C**: Clear screen (removes artifacts)
- **Q**: Quit game

## 🎨 Display Features

### Windows/GTK Forms Version
- Clean, modern interface
- Status bar showing game state and timer
- Intuitive menu system
- Custom icons for mines and flags
- Responsive grid that adjusts to window size
- Color-coded numbers:
  - Blue for 1
  - Green for 2
  - Red for 3
  - Dark Blue for 4
  - Dark Red for 5
  - Dark Gray for higher numbers

### Terminal Version
- Bold, colored numbers (1-8)
- High contrast mine and flag symbols
- Reverse highlighting for cursor position
- Clean, intuitive UI with status messages
- Animated title banner
- Current seed display (for sharing/replaying)

## 🛠️ Building

### Windows Forms Version
Requirements:
- Windows operating system
- .NET Framework 4.5 or higher
- 64-bit processor
- Minimum 2GB RAM
- Screen resolution: 800x600 or higher

``` cmd
cd windows
msbuild /p:Configuration=Release /p:Platform=x64 minesweeper.vcxproj
```

### GTK Version
Requirements:
- Linux operating system
- G++
- gtk3-development libraries (gtk3-devel on Fedora)
- Minimum 2GB RAM
- Screen resolution: 800x600 or higher

```bash
# Compile and run GTK version
cd gtk
make
cd build/linux
./minesweeper
```
### Windows GTK Version

Requirements:
- Linux operating system (to compile)
- G++
- 64-bit processor
- Minimum 2GB RAM
- Screen resolution: 800x600 or higher
- MINGW and GTK3-development (mingw-gtk3 on Fedora)

```bash
# Compile and run GTK version
cd gtk
make windows
cd build
mv windows minesweeper_gtk
zip minesweeper_gtk.zip minesweeper_gtk -r
```

```
# Copy ZIP file to windows and decompress
On windows run minesweeper_gtk.exe
```

### Terminal Version
Requirements:
- C++ compiler with C++11 support
- ncurses library

```bash
# Compile and run Terminal version
cd CLI
make
./minesweeper
```

## 🎲 Gameplay Tips

1. The first click is always safe - use it strategically!
2. Numbers indicate how many mines are in the adjacent cells
3. Flag suspected mines to keep track
4. Use quick-reveal on numbers when you've flagged all nearby mines
5. Try to beat your best times - they're saved per difficulty level

## 🔄 Game States

All versions support:
- Active gameplay
- Game over (mine revealed)
- Victory (all non-mine cells revealed)
- High score entry
- High score display

The Terminal version also includes:
- Menu (difficulty selection)
- Help screen

## 🎥 Terminal Version Preview

```
MINESWEEPER                  Time: 01:23
1 1 1
1 * 1    # = Hidden cell
1 1 1    F = Flag
         * = Mine
```

## 📝 License
All versions are licensed under [MIT](LICENSE.md); see [LICENSE.md](LICENSE.md).  

## ✉️  Contact Author
Created by Jason Brian Hall ([jasonbrianhall@gmail.com](mailto:jasonbrianhall@gmail.com))


## 🎮 Other Projects: The Coding Playground

Bored? Let me rescue you from the depths of monotony with these digital delights! 🚀

🃏 **Solitaire Insanity**: [Solitaire & Freecell](https://github.com/jasonbrianhall/solitaire) - A meticulously crafted digital card game that brings both Klondike Solitaire and Freecell to life! Featuring smooth animations, custom card decks, multiple drawing modes, and keyboard-friendly controls. Whether you're a classic Solitaire fan or a Freecell strategist, this game offers a delightful mix of challenge and entertainment! 🃏✨ (Prepare to get hooked!) 🎮

🧩 **Sudoku Solver Spectacular**: [Sudoku Solver](https://github.com/jasonbrianhall/sudoku_solver) - A Sudoku Swiss Army Knife! 🚀 This project is way more than just solving puzzles. Dive into a world where:
- 🧠 Puzzle Generation: Create brain-twisting Sudoku challenges
- 📄 MS-Word Magic: Generate professional puzzle documents
- 🚀 Extreme Solver: Crack instantaneously the most mind-bending Sudoku puzzles
- 🎮 Bonus Game Mode: Check out the playable version hidden in python_generated_puzzles

Numbers have never been this exciting! Prepare for a Sudoku adventure that'll make your brain cells do a happy dance! 🕺

🧊 **Rubik's Cube Chaos**: [Rubik's Cube Solver](https://github.com/jasonbrianhall/rubikscube/) - Crack the code of the most mind-bending 3x3 puzzle known to humanity! Solving optional, frustration guaranteed! 😅

🐛 **Willy the Worm's Wild Ride**: [Willy the worm](https://github.com/jasonbrianhall/willytheworm) - A 2D side-scroller starring the most adventurous invertebrate in gaming history! Who said worms can't be heroes? 🦸‍♂️

🧙‍♂️ **The Wizard's Castle: Choose Your Own Adventure**: [The Wizard's Castle](https://github.com/jasonbrianhall/wizardscastle) - A Text-Based RPG that works on QT5, CLI, and even Android! Magic knows no boundaries! ✨

🔤 **Hangman Hijinks**: [Hangman](https://github.com/jasonbrianhall/hangman) - Word-guessing mayhem in your terminal! Prepare for linguistic warfare! 💬

🕹️ **Bonus Level**: I've got a treasure trove of [more projects](https://github.com/jasonbrianhall) just waiting to be discovered! Some are shiny and new, some are old code that might need a digital retirement party. It's like a coding yard sale - you never know what gems you'll find! 🏴‍☠️

*Warning: Prolonged exposure may cause uncontrollable coding inspiration and spontaneous nerd moments* 🤓✨
