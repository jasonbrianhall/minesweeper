# 🎮 Terminal Minesweeper

A modern, feature-rich implementation of the classic Minesweeper game for terminal environments, built with ncurses.

## ✨ Features

- Three difficulty levels:
  - Easy (9x9 grid, 10 mines)
  - Medium (16x16 grid, 40 mines)
  - Hard (16x30 grid, 99 mines)
- Color-coded numbers for adjacent mine counts
- Game timer
- Animated title banner
- Flag placement system
- Cursor-based navigation
- First-click protection (never hit a mine on first move)
- Seed support for reproducible games
- Quick-reveal for completed number tiles

## 🎯 Controls

- **Arrow Keys**: Move cursor
- **Space**: Reveal cell
- **F**: Flag/unflag cell
- **H**: Show/hide help menu
- **N**: Start new game
- **R**: Restart current game
- **C**: Clear screen (removes artifacts)
- **Q**: Quit game

## 🛠️ Building

Requirements:
- C++ compiler with C++11 support
- ncurses library

```bash
# Compile and run
./compile.sh
./minesweeper
```

## 🎲 Gameplay Tips

1. The first click is always safe - use it strategically!
2. Numbers indicate how many mines are in the adjacent cells
3. Flag suspected mines with F to keep track
4. Click revealed numbers to quickly uncover adjacent cells if you've flagged all nearby mines
5. Use seeds to challenge friends with the same mine layout

## 🎨 Display Features

- Bold, colored numbers (1-8)
- High contrast mine and flag symbols
- Reverse highlighting for cursor position
- Animated title banner
- Clean, intuitive UI with status messages
- Game timer display
- Current seed display (for sharing/replaying)

## 🔄 Game States

- Menu (difficulty selection)
- Help screen
- Active gameplay
- Win/lose conditions with stats

## 🎥 Preview

```
MINESWEEPER                  Time: 01:23
1 1 1
1 * 1    # = Hidden cell
1 1 1    F = Flag
         * = Mine
```

## 📝 License

Free to use, modify, and distribute. Share your improvements!
