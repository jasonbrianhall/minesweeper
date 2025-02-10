#pragma once
#include <string>
#include <vector>

struct Score {
    std::string name;
    int time;
    std::string difficulty;
};

class Highscores {
public:
    Highscores();
    void addScore(const Score& score);
    const std::vector<Score>& getScores() const;

private:
    void loadScores();
    void saveScores();
    
    std::string scorePath;
    std::vector<Score> scores;
    const size_t MAX_SCORES = 10;
};
