#pragma once

#include <Windows.h>
#include <string>
#include "highscores.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

enum class Difficulty {
    EASY,
    MEDIUM,
    HARD
};

class GameTimer {
public:
    void start();
    void stop();
    std::string getTimeString() const;
};

class Minesweeper {
public:
    Minesweeper();
    bool firstMove;
    bool gameOver;
    bool won;
    int width;
    int height;
    std::string playerName;
    GameTimer timer;
    Highscores highscores;
    std::vector<std::vector<bool>> minefield;
    std::vector<std::vector<bool>> revealed;
    std::vector<std::vector<bool>> flagged;

    void setDifficulty(Difficulty diff);
    void reset();
    void initializeMinefield(int row, int col);
    int countAdjacentMines(int row, int col);
    void revealCell(int row, int col);
    void revealAllMines();
    bool checkWin();
    bool isHighScore(int time);
    void saveHighscore();
};

namespace MinesweeperGame {
    public ref class MinesweeperWrapper {
    private:
        Minesweeper* nativeMinesweeper;

    public:
        MinesweeperWrapper();
        ~MinesweeperWrapper();

        property Minesweeper* NativeMinesweeper {
            Minesweeper* get();
        }

        void SetDifficulty(int difficulty);
        bool IsHighScore(int time);
        void SaveHighScore(String^ name);
        System::Collections::Generic::List<String^>^ GetHighScores();
        void RevealCell(int row, int col);
        void ToggleFlag(int row, int col);
        bool IsRevealed(int row, int col);
        bool IsFlagged(int row, int col);
        bool IsMine(int row, int col);
        int GetAdjacentMines(int row, int col);
        bool IsGameOver();
        bool HasWon();
        String^ GetTime();
        void Reset();
        int GetWidth();
        int GetHeight();
    };

    public ref class MainForm : public System::Windows::Forms::Form {
    private:
        MinesweeperWrapper^ minesweeper;
        array<Button^, 2>^ grid;
        MenuStrip^ menuStrip;
        ToolStrip^ toolStrip;
        StatusStrip^ statusStrip;
        ToolStripStatusLabel^ statusLabel;
        ToolStripStatusLabel^ timeLabel;
        TextBox^ instructionsBox;
        System::Drawing::Font^ buttonFont;
        System::Windows::Forms::Timer^ gameTimer;
        Form^ highScoreForm;
        TextBox^ nameEntryBox;
        ListView^ highScoreList;

        void InitializeComponent();
        void UpdateTimer(Object^ sender, EventArgs^ e);
        void MainForm_KeyDown(Object^ sender, KeyEventArgs^ e);
        void InitializeGrid();
        void UpdateCell(int row, int col);
        void UpdateAllCells();
        void Cell_MouseUp(Object^ sender, MouseEventArgs^ e);
        void ShowHighScoreEntry();
        void SubmitHighScore(Object^ sender, EventArgs^ e);
        void ShowHighScores();
        void CloseHighScores(Object^ sender, EventArgs^ e);
        void NewGame_Click(Object^ sender, EventArgs^ e);
        void Exit_Click(Object^ sender, EventArgs^ e);
        void SetEasy_Click(Object^ sender, EventArgs^ e);
        void SetMedium_Click(Object^ sender, EventArgs^ e);
        void SetHard_Click(Object^ sender, EventArgs^ e);
        void UpdateStatus(String^ message);

    public:
        MainForm();
    };
}
