#include "game_state.h"
#include <iostream>

GameManager::GameManager() {
    current_state = GameState::MENU;
    score = 0;
    health = 100;
    max_health = 100;
    game_time = 0.0f;
}

void GameManager::StartGame() {
    current_state = GameState::PLAYING;
    score = 0;
    health = max_health;
    game_time = 0.0f;
    std::cout << "Game Started!" << std::endl;
}

void GameManager::PauseGame() {
    if (current_state == GameState::PLAYING) {
        current_state = GameState::PAUSED;
        std::cout << "Game Paused" << std::endl;
    }
}

void GameManager::ResumeGame() {
    if (current_state == GameState::PAUSED) {
        current_state = GameState::PLAYING;
        std::cout << "Game Resumed" << std::endl;
    }
}

void GameManager::GameOver() {
    current_state = GameState::GAME_OVER;
    std::cout << "GAME OVER! Final Score: " << score << std::endl;
}

void GameManager::AddScore(int points) {
    score += points;
}

void GameManager::TakeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
    std::cout << "Ship hit! Health: " << health << "/" << max_health << std::endl;

    if (health <= 0) {
        GameOver();
    }
}

bool GameManager::IsPlaying() const {
    return current_state == GameState::PLAYING;
}
