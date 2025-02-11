#include "highscores.h"
#include <fstream>
#include <algorithm>
#include <cstdlib>

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(dir) _mkdir(dir)
    #define PATH_SEP "\\"
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MKDIR(dir) mkdir(dir, 0700)
    #define PATH_SEP "/"
#endif

Highscores::Highscores() {
    #ifdef _WIN32
        const char* home = getenv("APPDATA");
    #else
        const char* home = getenv("HOME");
    #endif

    if (!home) {
        home = ".";
    }

    // Create the .minesweeper directory path
    std::string dirPath = std::string(home) + PATH_SEP + ".minesweeper";
    
    // Create directory using appropriate API
    MKDIR(dirPath.c_str());
    
    // Set the scores file path inside the .minesweeper directory
    scorePath = dirPath + PATH_SEP + "scores.txt";
    loadScores();
}

void Highscores::addScore(const Score& score) {
    scores.push_back(score);
    std::sort(scores.begin(), scores.end(), 
        [](const Score& a, const Score& b) {
            if (a.difficulty != b.difficulty) return a.difficulty < b.difficulty;
            return a.time < b.time;
        });
    
    if (scores.size() > MAX_SCORES) {
        scores.resize(MAX_SCORES);
    }
    saveScores();
}

const std::vector<Score>& Highscores::getScores() const { 
    return scores; 
}

void Highscores::loadScores() {
    std::ifstream file(scorePath);
    if (!file) return;
    
    scores.clear();
    std::string line;
    while (std::getline(file, line)) {
        size_t pos1 = line.find('|');
        size_t pos2 = line.find('|', pos1 + 1);
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
            Score score;
            score.name = line.substr(0, pos1);
            score.time = std::stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
            score.difficulty = line.substr(pos2 + 1);
            scores.push_back(score);
        }
    }
}

void Highscores::saveScores() {
    std::ofstream file(scorePath);
    if (!file) return;
    
    for (const auto& score : scores) {
        file << score.name << '|' << score.time << '|' << score.difficulty << '\n';
    }
}
