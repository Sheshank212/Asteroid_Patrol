#ifndef GAME_STATE_H
#define GAME_STATE_H

// Game state enumeration
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

// Game manager class
class GameManager {
public:
    GameState current_state;
    int score;
    int health;
    int max_health;
    float game_time;

    GameManager();
    void StartGame();
    void PauseGame();
    void ResumeGame();
    void GameOver();
    void AddScore(int points);
    void TakeDamage(int damage);
    bool IsPlaying() const;
};

#endif // GAME_STATE_H
