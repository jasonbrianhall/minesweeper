# 🎮 Minesweeper

A modern, feature-rich implementation of the classic Minesweeper game, available in Windows Forms, Linux GTK, and Terminal versions.

## 📦 Versions

### Windows Forms Version
A polished, graphical implementation built with Windows Forms and C++, featuring a modern user interface.

### Linux GTK Version
A polished, graphical implementation built with GTK and C++, featuring a modern user interface.

### Terminal Version
A classic, ncurses-based implementation perfect for terminal environments and systems without graphical interfaces.

## ✨ Common Features

Both versions include:
- Three difficulty levels:
  - Easy (9x9 grid, 10 mines)
  - Medium (16x16 grid, 40 mines)
  - Hard (16x30 grid, 99 mines)
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
- 64-bit processor
- Minimum 2GB RAM
- Screen resolution: 800x600 or higher

```bash
# Compile and run GTK version
cd gtk
make
./minesweeper
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
