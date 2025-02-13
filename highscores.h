#pragma once
#include <string>
#include <vector>
#include <map>

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
    std::vector<Score> getScoresByDifficulty(const std::string& difficulty) const;
    bool isHighScore(int time);

private:
    void loadScores();
    void saveScores();
    
    std::string scorePath;
    std::vector<Score> scores;  // All scores
    std::map<std::string, std::vector<Score>> scoresByDifficulty;  // Scores grouped by difficulty
    const size_t MAX_SCORES_PER_DIFFICULTY = 10;
};

