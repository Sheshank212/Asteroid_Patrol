#include "ui/menu_manager.h"
#include "ui/button.h"
#include "ui/text_renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <vector>
#include <memory>

// Inline shader sources for UI quad rendering
const char* UI_VERTEX_SHADER = R"(
#version 330 core
layout (location = 0) in vec2 position;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(position, 0.0, 1.0);
}
)";

const char* UI_FRAGMENT_SHADER = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 color;

void main() {
    FragColor = vec4(color, 1.0);
}
)";

// Inline shader sources for text rendering
const char* TEXT_VERTEX_SHADER = R"(
#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)";

const char* TEXT_FRAGMENT_SHADER = R"(
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
)";

MenuManager::MenuManager(int window_width, int window_height)
    : window_width_(window_width), window_height_(window_height),
      current_menu_(MenuState::MAIN_MENU),
      text_shader_program_(0), ui_shader_program_(0),
      game_over_score_(0), game_over_wave_(0) {
}

MenuManager::~MenuManager() {
    if (text_shader_program_) glDeleteProgram(text_shader_program_);
    if (ui_shader_program_) glDeleteProgram(ui_shader_program_);
}

bool MenuManager::Initialize() {
    // Load UI shaders for button quads
    ui_shader_program_ = LoadShaders(UI_VERTEX_SHADER, UI_FRAGMENT_SHADER);
    if (ui_shader_program_ == 0) {
        std::cerr << "Failed to load UI shaders" << std::endl;
        return false;
    }

    // Set projection matrix for UI shader (orthographic, screen coordinates)
    glm::mat4 ui_projection = glm::ortho(0.0f, (float)window_width_, 0.0f, (float)window_height_);
    glUseProgram(ui_shader_program_);
    glUniformMatrix4fv(glGetUniformLocation(ui_shader_program_, "projection"), 1, GL_FALSE, &ui_projection[0][0]);

    // Load text shaders
    text_shader_program_ = LoadShaders(TEXT_VERTEX_SHADER, TEXT_FRAGMENT_SHADER);
    if (text_shader_program_ == 0) {
        std::cerr << "Failed to load text shaders" << std::endl;
        return false;
    }

    // Initialize text renderer
    text_renderer_ = std::make_unique<TextRenderer>();
    if (!text_renderer_->Initialize(text_shader_program_)) {
        std::cerr << "Failed to initialize text renderer" << std::endl;
        return false;
    }

    // Set orthographic projection for text rendering
    glm::mat4 text_projection = glm::ortho(0.0f, static_cast<float>(window_width_),
                                           0.0f, static_cast<float>(window_height_));
    text_renderer_->SetProjection(text_projection);

    // Create button layouts for all menus
    CreateMainMenuButtons();
    CreatePauseMenuButtons();
    CreateGameOverButtons();
    CreateSettingsButtons();

    return true;
}

void MenuManager::CreateMainMenuButtons() {
    float center_x = window_width_ / 2.0f;
    float start_y = window_height_ / 2.0f;
    float button_spacing = 80.0f;

    // Play Button
    auto play_btn = std::make_shared<Button>("PLAY GAME", center_x, start_y, 200, 50);
    play_btn->SetCallback([this]() {
        if (start_game_callback_) start_game_callback_();
    });
    main_menu_buttons_.push_back(play_btn);

    // Instructions Button
    auto instructions_btn = std::make_shared<Button>("INSTRUCTIONS", center_x, start_y - button_spacing, 200, 50);
    instructions_btn->SetCallback([this]() {
        current_menu_ = MenuState::INSTRUCTIONS;
    });
    main_menu_buttons_.push_back(instructions_btn);

    // Settings Button
    auto settings_btn = std::make_shared<Button>("SETTINGS", center_x, start_y - button_spacing * 2, 200, 50);
    settings_btn->SetCallback([this]() {
        current_menu_ = MenuState::SETTINGS;
    });
    main_menu_buttons_.push_back(settings_btn);

    // High Scores Button
    auto scores_btn = std::make_shared<Button>("HIGH SCORES", center_x, start_y - button_spacing * 3, 200, 50);
    scores_btn->SetCallback([this]() {
        current_menu_ = MenuState::HIGH_SCORES;
    });
    main_menu_buttons_.push_back(scores_btn);

    // Quit Button
    auto quit_btn = std::make_shared<Button>("QUIT", center_x, start_y - button_spacing * 4, 200, 50);
    quit_btn->SetCallback([this]() {
        if (quit_game_callback_) quit_game_callback_();
    });
    main_menu_buttons_.push_back(quit_btn);
}

void MenuManager::CreatePauseMenuButtons() {
    float center_x = window_width_ / 2.0f;
    float start_y = window_height_ / 2.0f;
    float button_spacing = 80.0f;

    // Resume Button
    auto resume_btn = std::make_shared<Button>("RESUME", center_x, start_y, 200, 50);
    resume_btn->SetCallback([this]() {
        if (resume_game_callback_) resume_game_callback_();
    });
    pause_menu_buttons_.push_back(resume_btn);

    // Settings Button
    auto settings_btn = std::make_shared<Button>("SETTINGS", center_x, start_y - button_spacing, 200, 50);
    settings_btn->SetCallback([this]() {
        current_menu_ = MenuState::SETTINGS;
    });
    pause_menu_buttons_.push_back(settings_btn);

    // Main Menu Button
    auto menu_btn = std::make_shared<Button>("MAIN MENU", center_x, start_y - button_spacing * 2, 200, 50);
    menu_btn->SetCallback([this]() {
        current_menu_ = MenuState::MAIN_MENU;
    });
    pause_menu_buttons_.push_back(menu_btn);

    // Quit Button
    auto quit_btn = std::make_shared<Button>("QUIT", center_x, start_y - button_spacing * 3, 200, 50);
    quit_btn->SetCallback([this]() {
        if (quit_game_callback_) quit_game_callback_();
    });
    pause_menu_buttons_.push_back(quit_btn);
}

void MenuManager::CreateGameOverButtons() {
    float center_x = window_width_ / 2.0f;
    float start_y = window_height_ / 2.0f - 150.0f;
    float button_spacing = 80.0f;

    // Restart Button
    auto restart_btn = std::make_shared<Button>("RESTART", center_x, start_y, 200, 50);
    restart_btn->SetCallback([this]() {
        if (restart_game_callback_) restart_game_callback_();
    });
    game_over_buttons_.push_back(restart_btn);

    // Main Menu Button
    auto menu_btn = std::make_shared<Button>("MAIN MENU", center_x, start_y - button_spacing, 200, 50);
    menu_btn->SetCallback([this]() {
        current_menu_ = MenuState::MAIN_MENU;
    });
    game_over_buttons_.push_back(menu_btn);

    // Quit Button
    auto quit_btn = std::make_shared<Button>("QUIT", center_x, start_y - button_spacing * 2, 200, 50);
    quit_btn->SetCallback([this]() {
        if (quit_game_callback_) quit_game_callback_();
    });
    game_over_buttons_.push_back(quit_btn);
}

void MenuManager::CreateSettingsButtons() {
    float center_x = window_width_ / 2.0f;
    float start_y = window_height_ / 2.0f + 100.0f;

    // Back Button
    auto back_btn = std::make_shared<Button>("BACK", center_x, start_y - 200.0f, 200, 50);
    back_btn->SetCallback([this]() {
        current_menu_ = MenuState::MAIN_MENU;
    });
    settings_buttons_.push_back(back_btn);
}

void MenuManager::Update(double mouse_x, double mouse_y) {
    // Mouse Y is already in screen space (flipped in main.cpp)
    double screen_y = mouse_y;

    // Update buttons based on current menu
    switch (current_menu_) {
        case MenuState::MAIN_MENU:
            for (auto& btn : main_menu_buttons_) {
                btn->Update(mouse_x, screen_y);
            }
            break;
        case MenuState::PAUSED:
            for (auto& btn : pause_menu_buttons_) {
                btn->Update(mouse_x, screen_y);
            }
            break;
        case MenuState::GAME_OVER:
            for (auto& btn : game_over_buttons_) {
                btn->Update(mouse_x, screen_y);
            }
            break;
        case MenuState::SETTINGS:
            for (auto& btn : settings_buttons_) {
                btn->Update(mouse_x, screen_y);
            }
            break;
        default:
            break;
    }
}

void MenuManager::HandleClick(double mouse_x, double mouse_y) {
    // Mouse Y is already in screen space (flipped in main.cpp)
    double screen_y = mouse_y;

    // Check buttons based on current menu
    switch (current_menu_) {
        case MenuState::MAIN_MENU:
            for (auto& btn : main_menu_buttons_) {
                if (btn->IsClicked(mouse_x, screen_y)) {
                    btn->Execute();
                    break;
                }
            }
            break;
        case MenuState::PAUSED:
            for (auto& btn : pause_menu_buttons_) {
                if (btn->IsClicked(mouse_x, screen_y)) {
                    btn->Execute();
                    break;
                }
            }
            break;
        case MenuState::GAME_OVER:
            for (auto& btn : game_over_buttons_) {
                if (btn->IsClicked(mouse_x, screen_y)) {
                    btn->Execute();
                    break;
                }
            }
            break;
        case MenuState::SETTINGS:
            for (auto& btn : settings_buttons_) {
                if (btn->IsClicked(mouse_x, screen_y)) {
                    btn->Execute();
                    break;
                }
            }
            break;
        default:
            break;
    }
}

void MenuManager::Render() {
    switch (current_menu_) {
        case MenuState::MAIN_MENU:
            RenderMainMenu();
            break;
        case MenuState::PAUSED:
            RenderPauseMenu();
            break;
        case MenuState::GAME_OVER:
            RenderGameOverScreen();
            break;
        case MenuState::INSTRUCTIONS:
            RenderInstructions();
            break;
        case MenuState::SETTINGS:
            RenderSettings();
            break;
        default:
            break;
    }
}

void MenuManager::RenderMainMenu() {
    // Render title
    text_renderer_->RenderText("ASTEROID PATROL",
                               window_width_ / 2.0f - 300.0f,
                               window_height_ - 150.0f,
                               2.0f,
                               glm::vec3(0.0f, 1.0f, 1.0f)); // Cyan

    text_renderer_->RenderText("A Space Survival Game",
                               window_width_ / 2.0f - 200.0f,
                               window_height_ - 200.0f,
                               1.0f,
                               glm::vec3(0.7f, 0.7f, 0.7f)); // Gray

    // Render buttons (using UI shader for quads)
    for (auto& btn : main_menu_buttons_) {
        btn->Render(text_renderer_.get(), ui_shader_program_);
    }
}

void MenuManager::RenderPauseMenu() {
    // Render pause title
    text_renderer_->RenderText("PAUSED",
                               window_width_ / 2.0f - 150.0f,
                               window_height_ - 150.0f,
                               2.0f,
                               glm::vec3(1.0f, 1.0f, 0.0f)); // Yellow

    // Render buttons
    for (auto& btn : pause_menu_buttons_) {
        btn->Render(text_renderer_.get(), ui_shader_program_);
    }
}

void MenuManager::RenderGameOverScreen() {
    // Render game over title
    text_renderer_->RenderText("GAME OVER",
                               window_width_ / 2.0f - 200.0f,
                               window_height_ - 150.0f,
                               2.5f,
                               glm::vec3(1.0f, 0.0f, 0.0f)); // Red

    // Render score and wave
    std::string score_text = "Final Score: " + std::to_string(game_over_score_);
    text_renderer_->RenderText(score_text,
                               window_width_ / 2.0f - 150.0f,
                               window_height_ / 2.0f + 50.0f,
                               1.5f,
                               glm::vec3(1.0f, 1.0f, 1.0f));

    std::string wave_text = "Wave Reached: " + std::to_string(game_over_wave_);
    text_renderer_->RenderText(wave_text,
                               window_width_ / 2.0f - 150.0f,
                               window_height_ / 2.0f,
                               1.5f,
                               glm::vec3(1.0f, 1.0f, 1.0f));

    // Render buttons
    for (auto& btn : game_over_buttons_) {
        btn->Render(text_renderer_.get(), ui_shader_program_);
    }
}

void MenuManager::RenderInstructions() {
    text_renderer_->RenderText("INSTRUCTIONS",
                               window_width_ / 2.0f - 200.0f,
                               window_height_ - 100.0f,
                               2.0f,
                               glm::vec3(0.0f, 1.0f, 1.0f));

    float y_pos = window_height_ - 200.0f;
    float line_height = 40.0f;
    float x_start = 100.0f;

    text_renderer_->RenderText("W/S - Forward/Backward", x_start, y_pos, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    y_pos -= line_height;
    text_renderer_->RenderText("A/D - Strafe Left/Right", x_start, y_pos, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    y_pos -= line_height;
    text_renderer_->RenderText("Arrow Keys - Rotate Ship", x_start, y_pos, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    y_pos -= line_height;
    text_renderer_->RenderText("SPACE - Fire Laser", x_start, y_pos, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    y_pos -= line_height;
    text_renderer_->RenderText("M - Fire Missile", x_start, y_pos, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    y_pos -= line_height;
    text_renderer_->RenderText("C - Toggle Camera View", x_start, y_pos, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    y_pos -= line_height;
    text_renderer_->RenderText("P - Pause Game", x_start, y_pos, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    y_pos -= line_height;
    text_renderer_->RenderText("Q - Quit Game", x_start, y_pos, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    text_renderer_->RenderText("Press ESC to go back", window_width_ / 2.0f - 150.0f, 100.0f, 1.0f, glm::vec3(0.7f, 0.7f, 0.7f));
}

void MenuManager::RenderSettings() {
    text_renderer_->RenderText("SETTINGS",
                               window_width_ / 2.0f - 150.0f,
                               window_height_ - 100.0f,
                               2.0f,
                               glm::vec3(0.0f, 1.0f, 1.0f));

    text_renderer_->RenderText("Settings coming soon...",
                               window_width_ / 2.0f - 200.0f,
                               window_height_ / 2.0f,
                               1.0f,
                               glm::vec3(0.7f, 0.7f, 0.7f));

    // Render back button
    for (auto& btn : settings_buttons_) {
        btn->Render(text_renderer_.get(), ui_shader_program_);
    }
}

GLuint MenuManager::CompileShader(const char* source, GLenum shader_type) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        std::cerr << "Shader compilation failed: " << info_log << std::endl;
        return 0;
    }

    return shader;
}

GLuint MenuManager::LoadShaders(const char* vertex_shader, const char* fragment_shader) {
    GLuint vs = CompileShader(vertex_shader, GL_VERTEX_SHADER);
    GLuint fs = CompileShader(fragment_shader, GL_FRAGMENT_SHADER);

    if (vs == 0 || fs == 0) {
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Check for linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        std::cerr << "Shader linking failed: " << info_log << std::endl;
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
