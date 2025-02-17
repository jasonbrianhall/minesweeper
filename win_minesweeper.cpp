#define NOMINMAX
#include <msclr/marshal_cppstd.h>
#include "win_minesweeper.h"
#include "highscores.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace System::Data;
using namespace System::Drawing;

namespace MinesweeperGame {

public ref class MinesweeperWrapper {
private:
    Minesweeper* nativeMinesweeper;
    int minCellSize = 30;  // Minimum cell size
    TextBox^ seedInput;
    int currentSeed=-1;

public:
    void setSeed(int seed) { currentSeed = seed; }
    int getSeed() { return currentSeed; }

    const std::vector<Score>& GetNativeHighscores() {
        return nativeMinesweeper->highscores.getScores(); // use nativeMinesweeper pointer
    }
    
    MinesweeperWrapper() { 
        nativeMinesweeper = new Minesweeper(); 
    }

    ~MinesweeperWrapper() {
        if (nativeMinesweeper) {
            delete nativeMinesweeper;
            nativeMinesweeper = nullptr;
        }
    }

    property Minesweeper* NativeMinesweeper {
        Minesweeper* get() { return nativeMinesweeper; }
    }
    
    int GetAdjacentFlags(int row, int col) {
        int count = 0;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int newY = row + dy;
                int newX = col + dx;
                if (newY >= 0 && newY < nativeMinesweeper->height && 
                    newX >= 0 && newX < nativeMinesweeper->width) {
                    if (nativeMinesweeper->flagged[newY][newX]) count++;
                }
            }
        }
        return count;
    }
    
    void RevealAdjacent(int row, int col) {
        if (!nativeMinesweeper->revealed[row][col]) return;
    
        int mineCount = GetAdjacentMines(row, col);
        int flagCount = GetAdjacentFlags(row, col);
    
        if (mineCount == flagCount) {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int newY = row + dy;
                    int newX = col + dx;
                    if (newY >= 0 && newY < nativeMinesweeper->height && 
                        newX >= 0 && newX < nativeMinesweeper->width) {
                        if (!nativeMinesweeper->flagged[newY][newX] && 
                            !nativeMinesweeper->revealed[newY][newX]) {
                            if (nativeMinesweeper->minefield[newY][newX]) {
                                nativeMinesweeper->gameOver = true;
                                nativeMinesweeper->revealAllMines();
                                nativeMinesweeper->timer.stop();
                                return;
                            }
                            nativeMinesweeper->revealCell(newY, newX);
                        }
                    }
                }
            }
            // Check win condition after revealing adjacent cells
            if (CheckWin()) {
                nativeMinesweeper->won = true;
                nativeMinesweeper->timer.stop();
            }
        }
    }    
    
    void SetDifficulty(int difficulty) {
        switch(difficulty) {
            case 0: // Easy
                nativeMinesweeper->setDifficulty(Difficulty::EASY);
                break;
            case 1: // Medium
                nativeMinesweeper->setDifficulty(Difficulty::MEDIUM);
                break;
            case 2: // Hard
                nativeMinesweeper->setDifficulty(Difficulty::HARD);
                break;
        }
    }

    bool IsHighScore(int time) {
        std::string difficulty;
        switch(nativeMinesweeper->width) {
            case 9:  difficulty = "Easy"; break;
            case 16: difficulty = "Medium"; break;
            case 30: difficulty = "Hard"; break;
            default: difficulty = "Custom"; break;
        }
        return nativeMinesweeper->isHighScore(time, difficulty);
    }

    void SaveHighScore(String^ name) {
        std::string stdName = msclr::interop::marshal_as<std::string>(name);
        nativeMinesweeper->playerName = stdName;
        nativeMinesweeper->saveHighscore();
    }

    List<String^>^ GetHighScores() {
        List<String^>^ scores = gcnew List<String^>();
        const auto& nativeScores = nativeMinesweeper->highscores.getScores();
        for (const auto& score : nativeScores) {
            String^ scoreStr = String::Format("{0,-20} {1,-10} {2,-10}",
                gcnew String(score.name.c_str()),
                (score.time / 60).ToString("D2") + ":" + (score.time % 60).ToString("D2"),
                gcnew String(score.difficulty.c_str()));
            scores->Add(scoreStr);
        }
        return scores;
    }

    void RevealCell(int row, int col) {
        if (nativeMinesweeper->firstMove) {
            nativeMinesweeper->initializeMinefield(row, col, getSeed());
            nativeMinesweeper->firstMove = false;
            nativeMinesweeper->timer.start();
        }
    
        if (!nativeMinesweeper->flagged[row][col]) {
            if (nativeMinesweeper->minefield[row][col]) {
                nativeMinesweeper->gameOver = true;
                nativeMinesweeper->revealAllMines();
                nativeMinesweeper->timer.stop();
            } else {
                nativeMinesweeper->revealCell(row, col);
                // Check win condition after revealing cell
                if (CheckWin()) {
                    nativeMinesweeper->won = true;
                    nativeMinesweeper->timer.stop();
                }
            }
        }
    }

    void ToggleFlag(int row, int col) {
        if (!nativeMinesweeper->revealed[row][col]) {
            nativeMinesweeper->flagged[row][col] = !nativeMinesweeper->flagged[row][col];
        }
    }

    bool IsRevealed(int row, int col) {
        return nativeMinesweeper->revealed[row][col];
    }

    bool IsFlagged(int row, int col) {
        return nativeMinesweeper->flagged[row][col];
    }

    bool IsMine(int row, int col) {
        return nativeMinesweeper->minefield[row][col];
    }

    int GetAdjacentMines(int row, int col) {
        return nativeMinesweeper->countAdjacentMines(row, col);
    }

    bool IsGameOver() {
        return nativeMinesweeper->gameOver;
    }

    bool HasWon() {
        return nativeMinesweeper->won;
    }

    String^ GetTime() {
        return gcnew String(nativeMinesweeper->timer.getTimeString().c_str());
    }

    void Reset() {
        nativeMinesweeper->reset();
    }

    int GetWidth() {
        return nativeMinesweeper->width;
    }

    int GetHeight() {
        return nativeMinesweeper->height;
    }
 
    bool CheckWin() {
        // Win condition: all non-mine cells are revealed
        for (int i = 0; i < nativeMinesweeper->height; i++) {
            for (int j = 0; j < nativeMinesweeper->width; j++) {
                // If a cell is not a mine and not revealed, game is not won
                if (!nativeMinesweeper->minefield[i][j] && !nativeMinesweeper->revealed[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
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
    System::Drawing::Font^ buttonFont;
    System::Windows::Forms::Timer^ gameTimer;    Form^ highScoreForm;
    TextBox^ nameEntryBox;
    ListView^ highScoreList;
    int minCellSize;
    TextBox^ seedInput;
    bool gameEndHandled = false;
    Label^ flagCounterBox;
    Label^ timerBox;

    Image^ flagImage;
    Image^ bombImage;
    Image^ revealedImage;
    static const char* FLAG_BASE64 = R"(iVBORw0KGgoAAAANSUhEUgAAACAAAAAgBAMAAACBVGfHAAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAdn
JLH8AAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAACFQTFRFAAAA/2Z
mAAAA////mTMzzMzMzGZmZjMzmWYzMwAA/8xmaoi8KgAAAAF0Uk5TAEDm2GYAAABdSURBVCjPY2DABpQ
U0AS6GggJKCkooepSVkkUTJnEwMAEE1VWFBQUFHOf7oIiAAIDLaDmXu6GIqAEdPaUREERuACYgeRBZfQ
AGqwCSmixwhQaGhqEIgBUocSAEwAAjwQWTza+izoAAAAASUVORK5CYII=)";

    static const char* BOMB_BASE64 = R"(iVBORw0KGgoAAAANSUhEUgAAACAAAAAgBAMAAACBVGfHAAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAd
nJLH8AAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAACFQTFRFAAAA
gwMTcwoRqBIlgTE0yyk14kFE21NWqnN1z5ye////XulGdQAAAAF0Uk5TAEDm2GYAAADbSURBVCjPbdK9
DoIwEAfwQ/lYKeEBsOEFTF10qm9AQqpxc7GzizAbN1cmeALCU0rLtUVClza/XPMvdwCsrNocfK63UBYI
JNdb5YBmukA+Ec40/4cdpRMI2DK2B49ogDq8RKJkjHgEY6AQGjg3+dEExL4sQLAVL4TYwAMhNSAQkiW4
iu/QsOPQO+hGOA29u3JrS3Zo3+k8tpvHavjMQKUMzQLasSIDJ+odqftYUJD6WFFVoghGiEmy4VMLrxCM
LYx9Yppc2K6rnkZS3u1c8sXk6BRi54Lg1mbtV/gBxfI7i3nTTAoAAAAASUVORK5CYII=)";

    static const char* REVEALED_BASE64 = R"(iVBORw0KGgoAAAANSUhEUgAAACAAAAAgAQMAAABJtOi3AAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAdnJLH8AAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAAAZQTFRFAAAAIx8gaVhvIAAAAAF0Uk5TAEDm2GYAAABWSURBVAjXY2DADRpghEITkDBoAxICLDBJ/g9AwoIPSCQcBin72MDAyPDDgYGJ44cCAwv/DwMGNhDBzP8jgYGx/cMBBobjD4FmFbQDFcvZAAn2BwxEAADI2BObaHDmJQAAAABJRU5ErkJggg==)";

    static const char* ICON_BASE64 = R"(AAABAAEAICAQAAEABADoAgAAFgAAACgAAAAgAAAAQAAAAAEABAAAAAAAAAIAAAAAAAAAAAAAEAAAABAAAABKNaUAhl1FAE5gdQDkljwAjXu8AHVy4AAfgN8AsaKMAGmwzgDt6NwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAmZmZmZmZmZmZmZmZmZmZmZmZd3d3d3d3d3d3d3d3eZmZcREREQAAACAAACEREReZmREUERBQAABmZmZhMzMzeZdxBgEQUAAAhmZmYXd3EXmXeIZmEAVVAIaGZmF3d3F5l3KIghBEBVCGKIZhmZlxeZdxdxEQWZQAhmJmZ3d3EXmXd3dxFFBQUIZmZmcXdxF5l3d3cRd0d3R5d3InF5cReZd3mXd4SIgABFSEhxFxEXmXl3d3eIiCEzMUiIcXdxF5l3mXmXKIEzMzMUiHF3cReZczNzM4gjMxEzMVh3FxcXmXF3d3coF3d3czNIAAAAB5l3d3d3gnd3d3czKAUAAAeZeWImB4h3d3d3M4hQF3EHmXl2ZgcoGZl3mTGIVHd3B5l3hmJ3iCmZmXMyiFCZlweZeYYodygilxETKIhVAUAHmXlyd3eIICeXGIiIURVVB5l5mZmXdIIndBiISXdERAeZdWZmZRERERIhF3EAAAAHmXZgZmYXERESIgd3VQAAB5l4BoZmd3cRF3Zmd1AFAAeZeAiYBnd3cRcImAdVSUUHmXiIiIZ3d3EXdoh3UERAB5mYiAiGd5l3F3cnd1VQVQmZl4iIiHmZmXeZmZdEREVJmZl3d3d3d3d3d3d3d3d3mZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA==)";

    void ViewHighScores_Click(Object^ sender, EventArgs^ e) {
        ShowHighScores();
    }

    void InitializeComponent() {
        this->Size = System::Drawing::Size(800, 600);
        this->Text = L"Minesweeper";
        this->StartPosition = FormStartPosition::CenterScreen;
    
        // Initialize StatusStrip with both status and time
        statusStrip = gcnew StatusStrip();
        statusLabel = gcnew ToolStripStatusLabel("Ready");
        timeLabel = gcnew ToolStripStatusLabel("Time: 00:00");
        statusStrip->Items->Add(statusLabel);
        statusStrip->Items->Add(timeLabel);
        this->Controls->Add(statusStrip);
    
        // Initialize game timer
        gameTimer = gcnew Timer();
        gameTimer->Interval = 100; // Update every 100ms for smoother display
        gameTimer->Tick += gcnew EventHandler(this, &MainForm::UpdateTimer);
        gameTimer->Start();
    
        // Initialize MenuStrip
        menuStrip = gcnew MenuStrip();
        ToolStripMenuItem^ fileMenu = gcnew ToolStripMenuItem("File");
        ToolStripMenuItem^ difficultyMenu = gcnew ToolStripMenuItem("Difficulty");
    
        fileMenu->DropDownItems->Add(gcnew ToolStripMenuItem(
            "New Game (N)", nullptr, 
            gcnew EventHandler(this, &MainForm::NewGame_Click)));
        fileMenu->DropDownItems->Add(gcnew ToolStripMenuItem(
            "Reset Game (R)", nullptr, 
            gcnew EventHandler(this, &MainForm::ResetGame_Click)));

        fileMenu->DropDownItems->Add(gcnew ToolStripMenuItem(
            "Exit", nullptr,
            gcnew EventHandler(this, &MainForm::Exit_Click)));
    
        // Create the game menu
        ToolStripMenuItem^ gameMenu = gcnew ToolStripMenuItem("&Game");
    
        ToolStripMenuItem^ highScoresMenuItem = gcnew ToolStripMenuItem(
            "&High Scores",
            nullptr,
            gcnew EventHandler(this, &MainForm::ViewHighScores_Click));
        highScoresMenuItem->ShortcutKeys = Keys::Control | Keys::H;
        highScoresMenuItem->ShowShortcutKeys = true;
    
        ToolStripMenuItem^ setSeedMenuItem = gcnew ToolStripMenuItem("&Set Seed", nullptr, gcnew EventHandler(this, &MainForm::EnterSeed_Click));
        setSeedMenuItem->ShortcutKeys = Keys::Control | Keys::S;
        setSeedMenuItem->ShowShortcutKeys = true;

    
        gameMenu->DropDownItems->Add(highScoresMenuItem);
        gameMenu->DropDownItems->Add(setSeedMenuItem);
    
        menuStrip->Items->Add(gameMenu);            
    
        // Add About menu
        ToolStripMenuItem^ helpMenu = gcnew ToolStripMenuItem("Help");
        helpMenu->DropDownItems->Add(gcnew ToolStripMenuItem(
            "How To Play", nullptr,
            gcnew EventHandler(this, &MainForm::ShowHowToPlay_Click)));
    
        helpMenu->DropDownItems->Add(gcnew ToolStripMenuItem(
            "About", nullptr,
            gcnew EventHandler(this, &MainForm::ShowAbout_Click)));
                
        difficultyMenu->DropDownItems->Add(gcnew ToolStripMenuItem(
            "Easy (9x9) F1", nullptr,
            gcnew EventHandler(this, &MainForm::SetEasy_Click)));
        difficultyMenu->DropDownItems->Add(gcnew ToolStripMenuItem(
            "Medium (16x16) F2", nullptr,
            gcnew EventHandler(this, &MainForm::SetMedium_Click)));
        difficultyMenu->DropDownItems->Add(gcnew ToolStripMenuItem(
            "Hard (30x16) F3", nullptr,
            gcnew EventHandler(this, &MainForm::SetHard_Click)));
    
        menuStrip->Items->Add(fileMenu);
        menuStrip->Items->Add(gameMenu);
        menuStrip->Items->Add(difficultyMenu);
        menuStrip->Items->Add(helpMenu);

        timerBox = gcnew Label();
        timerBox->AutoSize = false;
        timerBox->Size = System::Drawing::Size(100, 30);
        timerBox->Location = Point(this->ClientSize.Width - 120, menuStrip->Height + 5);
        timerBox->TextAlign = ContentAlignment::MiddleCenter;
        timerBox->BorderStyle = BorderStyle::FixedSingle;
        timerBox->BackColor = Color::White;
        timerBox->Font = gcnew System::Drawing::Font(L"Consolas", 16, FontStyle::Bold);  // Changed to Consolas as it's more commonly available
        timerBox->Text = "00:00";  // Initialize the timer text
        timerBox->Click += gcnew EventHandler(this, &MainForm::TimerBox_Click);
        this->Controls->Add(timerBox);

        flagCounterBox = gcnew Label();
        flagCounterBox->AutoSize = false;
        flagCounterBox->Size = System::Drawing::Size(100, 30);
        flagCounterBox->Location = Point(this->ClientSize.Width - 120, menuStrip->Height + timerBox->Height + 10);
        flagCounterBox->TextAlign = ContentAlignment::MiddleCenter;
        flagCounterBox->BorderStyle = BorderStyle::FixedSingle;
        flagCounterBox->BackColor = Color::White;
        flagCounterBox->Font = gcnew System::Drawing::Font(L"Consolas", 16, FontStyle::Bold);
        // This code was added as a cheat to check the highscores functionality
        //flagCounterBox->Click += gcnew EventHandler(this, &MainForm::FlagCounterBox_Click);
        this->Controls->Add(flagCounterBox);

    
        this->MainMenuStrip = menuStrip;
        this->Controls->Add(menuStrip);
    
        // Handle keyboard shortcuts
        this->KeyPreview = true;
        this->KeyDown += gcnew KeyEventHandler(this, &MainForm::MainForm_KeyDown);
    
        InitializeGrid();
    }

    void FlagCounterBox_Click(Object^ sender, EventArgs^ e) {
        // Force timer to stop
        //gameTimer->stop();
        // Show high score entry dialog
        ShowHighScoreEntry();
    }

    void UpdateTimer(Object^ sender, EventArgs^ e) {
        // Update flag counter
        int totalBombs=0;
        switch(minesweeper->GetWidth()) {
            case 9:  totalBombs = 10; break;  // Easy
            case 16: totalBombs = 40; break;  // Medium
            case 30: totalBombs = 99; break;  // Hard
        }
        
        int flagCount = 0;
        for (int i = 0; i < minesweeper->GetHeight(); i++) {
            for (int j = 0; j < minesweeper->GetWidth(); j++) {
               if (minesweeper->IsFlagged(i, j)) flagCount++;
            }
        }
        
        flagCounterBox->Text = (totalBombs - flagCount).ToString();

        if (!minesweeper->IsGameOver() && !minesweeper->HasWon()) {
            if (!minesweeper->NativeMinesweeper->firstMove) {
                String^ time = minesweeper->GetTime();
                timerBox->Text = time;
                timeLabel->Text = "Time: " + time;
            } else {
                timerBox->Text = "00:00";
                timeLabel->Text = "Time: 00:00";
            }       
        }

        if (minesweeper->IsGameOver() || minesweeper->HasWon()) {
            HandleGameEnd();
        }
        statusStrip->Refresh();

    }
    
    void HandleGameEnd() {
        if (gameEndHandled) return;
        gameEndHandled = true;
        
        if (minesweeper->IsGameOver()) {
            UpdateStatus("Game Over!");
        } else if (minesweeper->HasWon()) {
            UpdateStatus("Congratulations! You've won!");
            if (minesweeper->IsHighScore(int::Parse(minesweeper->GetTime()->Split(':')[0]) * 60 + 
                int::Parse(minesweeper->GetTime()->Split(':')[1]))) {
                ShowHighScoreEntry();
            } else {
                ShowHighScores();
            }
        }
    }


    void MainForm_KeyDown(Object^ sender, KeyEventArgs^ e) {
        switch (e->KeyCode) {
            case Keys::F1:
                SetEasy_Click(nullptr, nullptr);
                break;
            case Keys::F2:
                SetMedium_Click(nullptr, nullptr);
                break;
            case Keys::F3:
                SetHard_Click(nullptr, nullptr);
                break;
            case Keys::N:
                NewGame_Click(nullptr, nullptr);
                break;
            case Keys::R:
                ResetGame_Click(nullptr, nullptr);
                break;

        }
    }

void InitializeGrid() {
    // Remove existing grid if any
    for each (Control^ control in this->Controls) {
        if (dynamic_cast<Panel^>(control) != nullptr) {
            this->Controls->Remove(control);
            break;
        }
    }

    int height = minesweeper->GetHeight();
    int width = minesweeper->GetWidth();
    
    // Calculate cell size based on height
    int availableHeight = this->ClientSize.Height - menuStrip->Height - 100;
    int cellSize = availableHeight / height;
    
    // Only adjust for width if in expert mode (30x16)
    if (width >= 30) {  // Expert mode width
        int availableWidth = this->ClientSize.Width - 140;  // Space before timer
        int widthBasedSize = availableWidth / width;
        if (widthBasedSize < cellSize) {
            cellSize = widthBasedSize;
        }
    }
    
    Panel^ gridPanel = gcnew Panel();
    gridPanel->Location = Point(20, menuStrip->Height + 25);
    gridPanel->Size = System::Drawing::Size(width * cellSize + 1, height * cellSize + 1);
    gridPanel->BackColor = Color::Gray;
    this->Controls->Add(gridPanel);

    grid = gcnew array<Button^, 2>(height, width);
    buttonFont = gcnew System::Drawing::Font(L"Lucida Console", cellSize / 3, FontStyle::Bold);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            grid[i, j] = gcnew Button();
            grid[i, j]->Size = System::Drawing::Size(cellSize - 1, cellSize - 1);
            grid[i, j]->Location = Point(j * cellSize, i * cellSize);
            grid[i, j]->Font = buttonFont;
            grid[i, j]->FlatStyle = FlatStyle::Standard;
            grid[i, j]->Tag = gcnew array<int>{i, j};
            grid[i, j]->MouseUp += gcnew MouseEventHandler(this, &MainForm::Cell_MouseUp);
            gridPanel->Controls->Add(grid[i, j]);
        }
    }
}
    
    void ShowHowToPlay_Click(Object^ sender, EventArgs^ e) {
        MessageBox::Show(
            L"How to Play Minesweeper:\n\n"
            L"- Left click to reveal a cell\n"
            L"- Right click to flag/unflag a cell\n"
            L"- Click on revealed number to reveal adjacent cells\n"
            L"  if correct number of flags are placed\n"
            L"- Press F1-F3 to change difficulty\n"
            L"- Press N for new game\n\n"
            L"The goal is to reveal all non-mine cells\n"
            L"without triggering any mines!",
            L"How to Play",
            MessageBoxButtons::OK,
            MessageBoxIcon::Information);
    }
    
    void TimerBox_Click(Object^ sender, EventArgs^ e) {
        NewGame_Click(nullptr, nullptr);  // Start new game when timer is clicked
    }
    
    
    void ShowAbout_Click(Object^ sender, EventArgs^ e) {
        MessageBox::Show(
            L"Minesweeper\n\n"
            L"A classic game where you must avoid the mines and clear the field.\n\n"
            L"Left click: Reveal cell\n"
            L"Right click: Flag/unflag cell\n"
            L"Click on revealed number: Reveal adjacent cells if correct number of flags\n\n"
            L"Written by Jason Hall (2025)",
            L"About Minesweeper",
            MessageBoxButtons::OK,
            MessageBoxIcon::Information);
    }
    
void EnterSeed_Click(Object^ sender, EventArgs^ e) {
    Form^ seedForm = gcnew Form();
    seedForm->Text = L"Enter Seed";
    seedForm->Size = System::Drawing::Size(300, 150);
    seedForm->StartPosition = FormStartPosition::CenterParent;
    seedForm->FormBorderStyle = Windows::Forms::FormBorderStyle::FixedDialog;
    
    Label^ instructionLabel = gcnew Label();
    instructionLabel->Text = L"Enter a number for the random seed:";
    instructionLabel->Location = Point(20, 15);
    instructionLabel->AutoSize = true;
    seedForm->Controls->Add(instructionLabel);
    
    seedInput = gcnew TextBox();
    seedInput->Location = Point(20, 40);
    seedInput->Size = System::Drawing::Size(240, 20);
    
    // Set the current seed as default text if it exists
    int currentSeed = minesweeper->getSeed();
    if (currentSeed >= 0) {
        seedInput->Text = currentSeed.ToString();
        seedInput->SelectAll();
    }
    
    Button^ okButton = gcnew Button();
    okButton->Text = L"OK";
    okButton->DialogResult = System::Windows::Forms::DialogResult::OK;
    okButton->Location = Point(85, 70);
    
    Button^ cancelButton = gcnew Button();
    cancelButton->Text = L"Cancel";
    cancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
    cancelButton->Location = Point(165, 70);
    
    seedForm->Controls->Add(seedInput);
    seedForm->Controls->Add(okButton);
    seedForm->Controls->Add(cancelButton);
    
    seedForm->AcceptButton = okButton;
    seedForm->CancelButton = cancelButton;
    
    if (seedForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        if (String::IsNullOrWhiteSpace(seedInput->Text)) {
            MessageBox::Show(L"Please enter a valid number", L"Invalid Input", 
                MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }
        
        try {
            int seed = Int32::Parse(seedInput->Text);
            if (seed < 0) {
                MessageBox::Show(L"Please enter a positive number", L"Invalid Input", 
                    MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }
            minesweeper->setSeed(seed);
            ResetGame_Click(nullptr, nullptr);
        }
        catch (FormatException^) {
            MessageBox::Show(L"Please enter a valid number", L"Invalid Input", 
                MessageBoxButtons::OK, MessageBoxIcon::Warning);
        }
        catch (OverflowException^) {
            MessageBox::Show(L"Number is too large", L"Invalid Input", 
                MessageBoxButtons::OK, MessageBoxIcon::Warning);
        }
    }
}

    void Cell_MouseUp(Object^ sender, MouseEventArgs^ e) {
        Button^ button = safe_cast<Button^>(sender);
        array<int>^ position = safe_cast<array<int>^>(button->Tag);
        int row = position[0];
        int col = position[1];

        if (minesweeper->IsGameOver() || minesweeper->HasWon()) {
            return;  
        }

        if (e->Button == System::Windows::Forms::MouseButtons::Left || (minesweeper->IsRevealed(row, col) && e->Button == System::Windows::Forms::MouseButtons::Right)) {
            // If clicking on a revealed number, check for auto-reveal
            if (minesweeper->IsRevealed(row, col) && !minesweeper->IsMine(row, col)) {
                int adjacentMines = minesweeper->GetAdjacentMines(row, col);
                if (adjacentMines > 0 && minesweeper->GetAdjacentFlags(row, col) == adjacentMines) {
                    minesweeper->RevealAdjacent(row, col);
                    UpdateAllCells();
                }
            } else {
                minesweeper->RevealCell(row, col);
                UpdateAllCells();
            }

        }
        else if (e->Button == System::Windows::Forms::MouseButtons::Right) {
            minesweeper->ToggleFlag(row, col);
            UpdateCell(row, col);
        }
    }

    void UpdateCell(int row, int col) {
        Button^ cell = grid[row, col];
        cell->Text = "";
        cell->Image = nullptr;
        
        if (minesweeper->IsRevealed(row, col)) {
            cell->FlatStyle = FlatStyle::Flat;  // Flat appearance for revealed cells
            cell->BackColor = SystemColors::Control;
        
            if (minesweeper->IsMine(row, col)) {
                cell->BackColor = Color::Red;
                if (bombImage) {
                    cell->Image = bombImage;
                    cell->ImageAlign = ContentAlignment::MiddleCenter;
                }
            } else {
                int count = minesweeper->GetAdjacentMines(row, col);
                if (count == 0 && revealedImage != nullptr) {
                    cell->Image = revealedImage;
                    cell->ImageAlign = ContentAlignment::MiddleCenter;
                } else if (count > 0) {
                    cell->Text = count.ToString();
                    switch (count) {
                        case 1: cell->ForeColor = Color::Blue; break;
                        case 2: cell->ForeColor = Color::Green; break;
                        case 3: cell->ForeColor = Color::Red; break;
                        case 4: cell->ForeColor = Color::DarkBlue; break;
                        case 5: cell->ForeColor = Color::DarkRed; break;
                        default: cell->ForeColor = Color::DarkGray; break;    
                    }
                }
            }
        } else {
            // Create raised border effect for unrevealed cells
            cell->FlatStyle = FlatStyle::Flat;
            cell->BackColor = Color::FromArgb(224, 224, 224); // #E0E0E0
            cell->FlatAppearance->BorderColor = Color::FromArgb(212, 212, 212); // #D4D4D4
            cell->FlatAppearance->BorderSize = 2;
            // Use BorderStyle property to create the raised effect
            cell->FlatAppearance->BorderColor = SystemColors::ButtonHighlight;
            cell->FlatAppearance->MouseOverBackColor = cell->BackColor;
            cell->FlatAppearance->MouseDownBackColor = SystemColors::ButtonShadow;

            if (minesweeper->IsFlagged(row, col) && flagImage) {
                cell->Image = flagImage;
                cell->ImageAlign = ContentAlignment::MiddleCenter;
            }
        }
    }
    
        
    void UpdateAllCells() {
        int height = minesweeper->GetHeight();
        int width = minesweeper->GetWidth();
        
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                UpdateCell(i, j);
            }
        }
    }

    void ShowHighScoreEntry() {
        highScoreForm = gcnew Form();
        highScoreForm->Text = "New High Score!";
        highScoreForm->Size = Drawing::Size(300, 150);
        highScoreForm->StartPosition = FormStartPosition::CenterParent;
        highScoreForm->FormBorderStyle = Windows::Forms::FormBorderStyle::FixedDialog;
        highScoreForm->MaximizeBox = false;
        highScoreForm->MinimizeBox = false;
    
        Label^ timeLabel = gcnew Label();
        timeLabel->Text = "Your time: " + minesweeper->GetTime();
        timeLabel->Location = Point(20, 20);
        timeLabel->AutoSize = true;
        highScoreForm->Controls->Add(timeLabel);
    
        nameEntryBox = gcnew TextBox();
        nameEntryBox->Location = Point(20, 50);
        nameEntryBox->Size = Drawing::Size(200, 20);
        nameEntryBox->MaxLength = 20;
        highScoreForm->Controls->Add(nameEntryBox);
    
        Button^ submitButton = gcnew Button();
        submitButton->Text = "Submit";
        submitButton->Location = Point(20, 80);
        submitButton->DialogResult = System::Windows::Forms::DialogResult::OK;
        highScoreForm->Controls->Add(submitButton);
    
        highScoreForm->AcceptButton = submitButton;
    
        if (highScoreForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            if (!String::IsNullOrWhiteSpace(nameEntryBox->Text)) {
                minesweeper->SaveHighScore(nameEntryBox->Text);
                ShowHighScores();
            }
        }
    }

    void SubmitHighScore(Object^ sender, EventArgs^ e) {
        if (!String::IsNullOrWhiteSpace(nameEntryBox->Text)) {
            minesweeper->SaveHighScore(nameEntryBox->Text);
            highScoreForm->Close();
            ShowHighScores();
        }
    }

    void ShowHighScores() {
        highScoreForm = gcnew Form();
        highScoreForm->Text = "High Scores";
        highScoreForm->MinimumSize = Drawing::Size(400, 300);
        highScoreForm->StartPosition = FormStartPosition::CenterParent;
        highScoreForm->MaximizeBox = true;
        highScoreForm->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Bottom | AnchorStyles::Left | AnchorStyles::Right);
    
        highScoreList = gcnew ListView();
        highScoreList->View = View::Details;
        highScoreList->Dock = DockStyle::Fill;
        highScoreList->Padding = System::Windows::Forms::Padding(20);
        highScoreList->FullRowSelect = true;
        highScoreList->GridLines = true;
        highScoreList->Columns->Add("Name", 150);
        highScoreList->Columns->Add("Time", 100);
        highScoreList->Columns->Add("Difficulty", 100);
    
        // Get the actual scores from the native Highscores class
        const std::vector<Score>& nativeScores = minesweeper->GetNativeHighscores();
        
        for (const auto& score : nativeScores) {
            int minutes = score.time / 60;
            int seconds = score.time % 60;
            String^ timeStr = String::Format("{0:D2}:{1:D2}", minutes, seconds);
            
            ListViewItem^ item = gcnew ListViewItem(gcnew array<String^> {
                gcnew String(score.name.c_str()),
                timeStr,
                gcnew String(score.difficulty.c_str())
            });
            highScoreList->Items->Add(item);
        }
    
        Panel^ mainPanel = gcnew Panel();
        mainPanel->Dock = DockStyle::Fill;
        mainPanel->Padding = System::Windows::Forms::Padding(20);
    
        Panel^ buttonPanel = gcnew Panel();
        buttonPanel->Height = 50;
        buttonPanel->Dock = DockStyle::Bottom;
    
        Button^ closeButton = gcnew Button();
        closeButton->Text = "Close";
        closeButton->AutoSize = true;
        closeButton->Anchor = static_cast<AnchorStyles>(AnchorStyles::None);
        closeButton->Location = Point((buttonPanel->Width - closeButton->Width) / 2, (buttonPanel->Height - closeButton->Height) / 2);
        closeButton->Click += gcnew EventHandler(this, &MainForm::CloseHighScores);
    
        buttonPanel->Controls->Add(closeButton);
        mainPanel->Controls->Add(highScoreList);
        mainPanel->Controls->Add(buttonPanel);
        highScoreForm->Controls->Add(mainPanel);
    
        highScoreForm->ShowDialog();
    }
    void CloseHighScores(Object^ sender, EventArgs^ e) {
        highScoreForm->Close();
    }

    void NewGame_Click(Object^ sender, EventArgs^ e) {
        Random^ rand = gcnew Random();
        int newSeed = rand->Next(0, Int32::MaxValue);
        minesweeper->setSeed(newSeed);
        if (seedInput != nullptr) {
            seedInput->Text = newSeed.ToString();
        }
        minesweeper->Reset();
        gameEndHandled = false;
        UpdateAllCells();
        UpdateStatus("New game started");
        gameTimer->Start();
        timerBox->Text = "00:00";        // Initialize the timer box
        timeLabel->Text = "Time: 00:00";
        statusStrip->Refresh();
    }

    void ResetGame_Click(Object^ sender, EventArgs^ e) {
        minesweeper->Reset();
        gameEndHandled = false;
        UpdateAllCells();
        UpdateStatus("Reset current game");
        gameTimer->Start();
        timerBox->Text = "00:00";        // Initialize the timer box
        timeLabel->Text = "Time: 00:00";
        statusStrip->Refresh();
    }


    void Exit_Click(Object^ sender, EventArgs^ e) {
        Application::Exit();
    }

    void SetEasy_Click(Object^ sender, EventArgs^ e) {
        minesweeper->SetDifficulty(0);
        InitializeGrid();
        UpdateStatus("Difficulty set to Easy");
        gameEndHandled = false;
    }

    void SetMedium_Click(Object^ sender, EventArgs^ e) {
        minesweeper->SetDifficulty(1);
        InitializeGrid();
        UpdateStatus("Difficulty set to Medium");
        gameEndHandled = false;
    }

    void SetHard_Click(Object^ sender, EventArgs^ e) {
        minesweeper->SetDifficulty(2);
        InitializeGrid();
        UpdateStatus("Difficulty set to Hard");
        gameEndHandled = false;
    }

    void UpdateStatus(String^ message) {
        statusLabel->Text = message;
        statusStrip->Refresh();
    }

    void MainForm_Resize(Object^ sender, EventArgs^ e) {
        timerBox->Location = Point(this->ClientSize.Width - 120, menuStrip->Height + 5);
        flagCounterBox->Location = Point(this->ClientSize.Width - 120, menuStrip->Height + timerBox->Height + 10);
        InitializeGrid();
        UpdateAllCells();
    }

    void LoadBase64Images() {
        try {
            // Convert base64 to image for flag
            array<Byte>^ flagBytes = Convert::FromBase64String(gcnew String(FLAG_BASE64));
            System::IO::MemoryStream^ flagStream = gcnew System::IO::MemoryStream(flagBytes);
            flagImage = Image::FromStream(flagStream);
    
            // Convert base64 to image for bomb
            array<Byte>^ bombBytes = Convert::FromBase64String(gcnew String(BOMB_BASE64));
            System::IO::MemoryStream^ bombStream = gcnew System::IO::MemoryStream(bombBytes);
            bombImage = Image::FromStream(bombStream);

            // Convert base64 to image for revealed
            array<Byte>^ revealedBytes = Convert::FromBase64String(gcnew String(REVEALED_BASE64));
            System::IO::MemoryStream^ revealedStream = gcnew System::IO::MemoryStream(revealedBytes);
            revealedImage = Image::FromStream(revealedStream);
        }
        catch (Exception^ ex) {
            MessageBox::Show("Error loading images: " + ex->Message);
        }
    }
    
    void SetApplicationIcon() {
        try {
            array<Byte>^ iconBytes = Convert::FromBase64String(gcnew String(ICON_BASE64));
            System::IO::MemoryStream^ iconStream = gcnew System::IO::MemoryStream(iconBytes);
            this->Icon = gcnew Drawing::Icon(iconStream);
        }
        catch (Exception^ ex) {
            MessageBox::Show("Error loading application icon: " + ex->Message);
        }
    }

public:
    MainForm() {
        minCellSize = 30;  // Initialize minCellSize
        LoadBase64Images();
        SetApplicationIcon();
        minesweeper = gcnew MinesweeperWrapper();
        InitializeComponent();
        this->Resize += gcnew EventHandler(this, &MainForm::MainForm_Resize);
        NewGame_Click(nullptr, nullptr);
    }
}; 

} // End of namespace MinesweeperGame

[STAThread]
int main(array<String^>^ args) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew MinesweeperGame::MainForm());
    return 0;
}
