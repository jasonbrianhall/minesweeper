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
        return nativeMinesweeper->isHighScore(time);
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
    TextBox^ instructionsBox;
    System::Drawing::Font^ buttonFont;
    System::Windows::Forms::Timer^ gameTimer;    Form^ highScoreForm;
    TextBox^ nameEntryBox;
    ListView^ highScoreList;
    int minCellSize;
    TextBox^ seedInput;
    bool gameEndHandled = false;

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

    static const char* ICON_BASE64 = R"(iVBORw0KGgoAAAANSUhEUgAAACAAAAAgBAMAAACBVGfHAAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAdnJLH8AAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAAB5QTFRFpTVKRV2GdWBOPJbkvHuN4HJ134AfjKKxzrBp3OjtsPRwLAAAAWNJREFUKM9l0cFPwjAUBvBigMTbGmbmUbkMj1ATOWqyRI41ARNvjEPHkc4tr7uNhW3lLLH2v/UVOBjsafllX/u1j9iLRf6DgT+rQFBat9aawtosiuYIutRJYSSkkC963BKjtU4AKKzbvBuRgphyXzrY7Mt8fuAO1iIBSUEkOeHuD9EXsaR0eAc5Jw7ehXijCBQ8cgQsA7MhvOidyaJnBBOCqq4KqXVNF27Tn1WbVg+STnTNfRdRIlQVVmWj+tpuXEQs0y1W/7qtX8HB93CpWrzX57TugIeA3yp0MClPPQKAdIvAHstTD/bEphUbD9hNK6VEKIxN9YCx8awNgLpTcA9dDZivzwAW1EyT7nzXUnkEkEEGWQFpGJgz+JzzRojkFJFAvY8DaVbCPRxCqisv6vFm1MTWumNVI7zegjSNiDHtIv2Oh51xj/gYsdSnDgQCYwwBH5jiJe4Iuac0cMO+nO3F9H8B6M7JKU9YozUAAAAASUVORK5CYII=)"

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
        gameTimer->Interval = 100; // Update every 100ms
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
            "Exit", nullptr,
            gcnew EventHandler(this, &MainForm::Exit_Click)));

        // Create the game menu
        ToolStripMenuItem^ gameMenu = gcnew ToolStripMenuItem("&Game");

        // Create menu items with proper shortcuts
        /* ToolStripMenuItem^ seedMenuItem = gcnew ToolStripMenuItem(
            "&Enter Seed...",
            nullptr,
            gcnew EventHandler(this, &MainForm::EnterSeed_Click)); 
        seedMenuItem->ShortcutKeys = Keys::Control | Keys::S;
        seedMenuItem->ShowShortcutKeys = true; */

        ToolStripMenuItem^ highScoresMenuItem = gcnew ToolStripMenuItem(
            "&High Scores",
            nullptr,
            gcnew EventHandler(this, &MainForm::ViewHighScores_Click));
        highScoresMenuItem->ShortcutKeys = Keys::Control | Keys::H;
        highScoresMenuItem->ShowShortcutKeys = true;

        // Add menu items to game menu
        //gameMenu->DropDownItems->Add(seedMenuItem);
        //gameMenu->DropDownItems->Add(gcnew ToolStripSeparator());  // Add separator
        gameMenu->DropDownItems->Add(highScoresMenuItem);

        // Add game menu to menu strip
        menuStrip->Items->Add(gameMenu);            

        // Add About menu
        ToolStripMenuItem^ helpMenu = gcnew ToolStripMenuItem("Help");
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

        this->MainMenuStrip = menuStrip;
        this->Controls->Add(menuStrip);

        instructionsBox = gcnew TextBox();
        instructionsBox->Multiline = true;
        instructionsBox->ReadOnly = true;
        instructionsBox->BackColor = System::Drawing::Color::LightBlue;
        instructionsBox->BorderStyle = BorderStyle::FixedSingle;
        instructionsBox->Location = Point(50, menuStrip->Height + 5);
        instructionsBox->Size = System::Drawing::Size(700, 80);
        instructionsBox->Text = L"Instructions:\r\n"
            L"  - Left click to reveal a cell\r\n"
            L"  - Right click to flag/unflag a cell\r\n"
            L"  - Press F1-F3 to change difficulty\r\n"
            L"  - Press N for new game";
        instructionsBox->Font = gcnew System::Drawing::Font(L"Lucida Console", 9);
        this->Controls->Add(instructionsBox);

        // Handle keyboard shortcuts
        this->KeyPreview = true;
        this->KeyDown += gcnew KeyEventHandler(this, &MainForm::MainForm_KeyDown);

        InitializeGrid();
    }

    void UpdateTimer(Object^ sender, EventArgs^ e) {
        timeLabel->Text = "Time: " + minesweeper->GetTime();
        if (minesweeper->IsGameOver() || minesweeper->HasWon()) {
            HandleGameEnd();
        }
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
        
        // Calculate cell size based on window size
        int availableWidth = this->ClientSize.Width - 100;  // Account for margins
        int availableHeight = this->ClientSize.Height - menuStrip->Height - instructionsBox->Height - 100;
        
        int cellSizeFromWidth = availableWidth / width;
        int cellSizeFromHeight = availableHeight / height;
        int cellSize = Math::Max(minCellSize, Math::Min(cellSizeFromWidth, cellSizeFromHeight));
        
        Panel^ gridPanel = gcnew Panel();
        int gridTop = menuStrip->Height + instructionsBox->Height + 25;
        gridPanel->Location = Point(50, gridTop);
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
                grid[i, j]->UseVisualStyleBackColor = true;
                grid[i, j]->Tag = gcnew array<int>{i, j};
                grid[i, j]->MouseUp += gcnew MouseEventHandler(this, &MainForm::Cell_MouseUp);
                gridPanel->Controls->Add(grid[i, j]);
            }
        }
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
        
        seedInput = gcnew TextBox();
        seedInput->Location = Point(20, 20);
        seedInput->Size = System::Drawing::Size(240, 20);
        
        Button^ okButton = gcnew Button();
        okButton->Text = L"OK";
        okButton->DialogResult = System::Windows::Forms::DialogResult::OK;
        okButton->Location = Point(100, 60);
        
        seedForm->Controls->Add(seedInput);
        seedForm->Controls->Add(okButton);
        seedForm->AcceptButton = okButton;
        
        if (seedForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            try {
                int seed = Int32::Parse(seedInput->Text);
                // Update the wrapper to handle seed
                minesweeper->setSeed(seed);
                NewGame_Click(nullptr, nullptr);
            }
            catch (...) {
                MessageBox::Show(L"Invalid seed value", L"Error", 
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
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

        if (e->Button == System::Windows::Forms::MouseButtons::Left) {
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
        cell->UseVisualStyleBackColor = false;
    
        if (minesweeper->IsRevealed(row, col)) {
            cell->BackColor = SystemColors::Control;
        
            if (minesweeper->IsMine(row, col)) {
                cell->BackColor = Color::Red;
                if (bombImage) {
                    cell->Image = bombImage;
                    cell->ImageAlign = ContentAlignment::MiddleCenter;
                }
            } else {
                int count = minesweeper->GetAdjacentMines(row, col);
                if (count == 0 && revealedImage != nullptr) {  // Check if eyeImage exists
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
        } else if (minesweeper->IsFlagged(row, col)) {
            cell->BackColor = Color::LightGray;
            if (flagImage) {
                cell->Image = flagImage;
                cell->ImageAlign = ContentAlignment::MiddleCenter;
            }
        } else {
            cell->BackColor = Color::LightGray;
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
        submitButton->Click += gcnew EventHandler(this, &MainForm::SubmitHighScore);
        highScoreForm->Controls->Add(submitButton);

        highScoreForm->ShowDialog();
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
        minesweeper->setSeed(-1);
        minesweeper->Reset();
        gameEndHandled = false;
        UpdateAllCells();
        UpdateStatus("New game started");
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
