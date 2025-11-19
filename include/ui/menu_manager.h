#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include <string>
#include <functional>

class Button;
class TextRenderer;

enum class MenuState {
    MAIN_MENU,
    SETTINGS,
    INSTRUCTIONS,
    HIGH_SCORES,
    PAUSED,
    GAME_OVER,
    NONE
};

/**
 * MenuManager - Handles all menu screens and UI navigation
 * Manages different menu states and button interactions
 */
class MenuManager {
public:
    MenuManager(int window_width, int window_height);
    ~MenuManager();

    // Initialize menu system
    bool Initialize();

    // Update menu (handle mouse position, hover effects)
    void Update(double mouse_x, double mouse_y);

    // Render current menu
    void Render();

    // Handle mouse click
    void HandleClick(double mouse_x, double mouse_y);

    // Get/Set current menu state
    MenuState GetCurrentMenu() const { return current_menu_; }
    void SetCurrentMenu(MenuState menu) { current_menu_ = menu; }

    // Callbacks for button actions
    void SetStartGameCallback(std::function<void()> callback) { start_game_callback_ = callback; }
    void SetQuitGameCallback(std::function<void()> callback) { quit_game_callback_ = callback; }
    void SetResumeGameCallback(std::function<void()> callback) { resume_game_callback_ = callback; }
    void SetRestartGameCallback(std::function<void()> callback) { restart_game_callback_ = callback; }

    // Get text renderer
    TextRenderer* GetTextRenderer() { return text_renderer_.get(); }

    // Set game over score
    void SetGameOverScore(int score, int wave) { game_over_score_ = score; game_over_wave_ = wave; }

private:
    int window_width_, window_height_;
    MenuState current_menu_;

    // UI Components
    std::unique_ptr<TextRenderer> text_renderer_;
    GLuint text_shader_program_;
    GLuint ui_shader_program_;

    // Buttons for different menus
    std::vector<std::shared_ptr<Button>> main_menu_buttons_;
    std::vector<std::shared_ptr<Button>> pause_menu_buttons_;
    std::vector<std::shared_ptr<Button>> game_over_buttons_;
    std::vector<std::shared_ptr<Button>> settings_buttons_;

    // Callbacks
    std::function<void()> start_game_callback_;
    std::function<void()> quit_game_callback_;
    std::function<void()> resume_game_callback_;
    std::function<void()> restart_game_callback_;

    // Game over data
    int game_over_score_;
    int game_over_wave_;

    // Helper functions
    void CreateMainMenuButtons();
    void CreatePauseMenuButtons();
    void CreateGameOverButtons();
    void CreateSettingsButtons();

    void RenderMainMenu();
    void RenderPauseMenu();
    void RenderGameOverScreen();
    void RenderInstructions();
    void RenderSettings();

    GLuint LoadShaders(const char* vertex_shader, const char* fragment_shader);
    GLuint CompileShader(const char* source, GLenum shader_type);
};

#endif // MENU_MANAGER_H
