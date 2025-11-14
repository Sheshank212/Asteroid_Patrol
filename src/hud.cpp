#include "hud.h"
#include "game_state.h"
#include <iostream>
#include <iomanip>

HUD::HUD() {
}

HUD::~HUD() {
}

void HUD::RenderMenu() {
    // For now, we'll use console output
    // In a full implementation, this would render text using OpenGL
    static bool menu_shown = false;
    if (!menu_shown) {
        std::cout << "\n========================================" << std::endl;
        std::cout << "         ASTEROID PATROL" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "  Press SPACE to start playing!" << std::endl;
        std::cout << "========================================\n" << std::endl;
        menu_shown = true;
    }
}

void HUD::RenderPaused() {
    static bool pause_shown = false;
    if (!pause_shown) {
        std::cout << "\n*** GAME PAUSED ***" << std::endl;
        std::cout << "Press P to resume" << std::endl;
        pause_shown = true;
    }
}

void HUD::RenderGameOver(int final_score) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "           GAME OVER!" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "     Final Score: " << final_score << std::endl;
    std::cout << "     Press R to restart" << std::endl;
    std::cout << "     Press Q to quit" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void HUD::RenderHUD(const GameManager* game) {
    // This displays in the console window
    // In-game overlays would require OpenGL text rendering

    // Display periodically (not every frame to avoid spam)
    static int frame_counter = 0;
    frame_counter++;

    if (frame_counter % 300 == 0) { // Every ~5 seconds at 60fps
        std::cout << "[HUD] Score: " << std::setw(6) << game->score
                  << " | Health: " << game->health << "/" << game->max_health << std::endl;
    }
}

void HUD::PrintText(int x, int y, const std::string& text) {
    // Simple console output helper
    std::cout << text;
}
