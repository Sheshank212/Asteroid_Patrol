#ifndef HUD_H
#define HUD_H

#include <string>

// Forward declaration
class GameManager;

// Simple HUD for displaying game information
class HUD {
public:
    HUD();
    ~HUD();

    void RenderMenu();
    void RenderPaused();
    void RenderGameOver(int final_score);
    void RenderHUD(const GameManager* game);

private:
    // Helper to print text at screen position (console output for now)
    void PrintText(int x, int y, const std::string& text);
};

#endif // HUD_H
