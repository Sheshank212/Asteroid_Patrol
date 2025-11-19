#include "ui/enhanced_hud.h"
#include "ui/text_renderer.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <vector>

// Inline shader for bar rendering
const char* BAR_VERTEX_SHADER = R"(
#version 330 core
layout (location = 0) in vec2 position;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(position, 0.0, 1.0);
}
)";

const char* BAR_FRAGMENT_SHADER = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 color;

void main() {
    FragColor = vec4(color, 1.0);
}
)";

EnhancedHUD::EnhancedHUD(int window_width, int window_height)
    : window_width_(window_width), window_height_(window_height),
      text_renderer_(nullptr), health_(100), max_health_(100),
      score_(0), wave_(1), combo_multiplier_(1),
      laser_ammo_(999), missile_ammo_(999), game_time_(0.0f),
      damage_flash_timer_(0.0f), low_health_pulse_(0.0f),
      bar_VAO_(0), bar_VBO_(0), bar_shader_program_(0) {
}

EnhancedHUD::~EnhancedHUD() {
    if (bar_VAO_) glDeleteVertexArrays(1, &bar_VAO_);
    if (bar_VBO_) glDeleteBuffers(1, &bar_VBO_);
    if (bar_shader_program_) glDeleteProgram(bar_shader_program_);
}

bool EnhancedHUD::Initialize(TextRenderer* text_renderer) {
    text_renderer_ = text_renderer;
    InitializeBarRendering();
    return true;
}

void EnhancedHUD::InitializeBarRendering() {
    // Create shader program for rendering bars
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &BAR_VERTEX_SHADER, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &BAR_FRAGMENT_SHADER, NULL);
    glCompileShader(fs);

    bar_shader_program_ = glCreateProgram();
    glAttachShader(bar_shader_program_, vs);
    glAttachShader(bar_shader_program_, fs);
    glLinkProgram(bar_shader_program_);

    glDeleteShader(vs);
    glDeleteShader(fs);

    // Create VAO and VBO for rendering quads
    glGenVertexArrays(1, &bar_VAO_);
    glGenBuffers(1, &bar_VBO_);

    glBindVertexArray(bar_VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, bar_VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);  // Reset shader program to avoid interference
}

void EnhancedHUD::ShowScorePopup(int score, float x, float y) {
    ScorePopup popup;
    popup.score = score;
    popup.x = x;
    popup.y = y;
    popup.lifetime = 0.0f;
    popup.max_lifetime = 2.0f;
    score_popups_.push_back(popup);
}

void EnhancedHUD::UpdatePopups(float delta_time) {
    // Update damage flash
    if (damage_flash_timer_ > 0.0f) {
        damage_flash_timer_ -= delta_time;
    }

    // Update low health pulse
    low_health_pulse_ += delta_time * 3.0f;

    // Update score popups
    for (auto it = score_popups_.begin(); it != score_popups_.end();) {
        it->lifetime += delta_time;
        it->y += 50.0f * delta_time; // Rise upward

        if (it->lifetime >= it->max_lifetime) {
            it = score_popups_.erase(it);
        } else {
            ++it;
        }
    }
}

void EnhancedHUD::Render() {
    if (!text_renderer_) return;

    // Enable blending for HUD elements
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    RenderHealthBar();
    RenderScore();
    RenderWaveInfo();
    RenderAmmo();
    RenderComboMultiplier();
    RenderGameTime();
    RenderCrosshair();
    RenderScorePopups();
    RenderDamageFlash();

    glDisable(GL_BLEND);
}

void EnhancedHUD::RenderHealthBar() {
    float bar_x = 20.0f;
    float bar_y = window_height_ - 50.0f;
    float bar_width = 200.0f;
    float bar_height = 20.0f;

    // Calculate health percentage
    float health_percent = std::max(0.0f, std::min(1.0f, static_cast<float>(health_) / max_health_));

    // Determine bar color based on health
    glm::vec3 bar_color;
    if (health_percent > 0.6f) {
        bar_color = glm::vec3(0.0f, 1.0f, 0.0f); // Green
    } else if (health_percent > 0.3f) {
        bar_color = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow
    } else {
        // Red with pulse effect when low
        float pulse = (std::sin(low_health_pulse_) + 1.0f) * 0.5f;
        bar_color = glm::vec3(1.0f, pulse * 0.3f, 0.0f);
    }

    // Render background bar (dark)
    RenderBar(bar_x, bar_y, bar_width, bar_height, 1.0f, glm::vec3(0.2f, 0.2f, 0.2f));

    // Render health bar
    RenderBar(bar_x, bar_y, bar_width, bar_height, health_percent, bar_color);

    // Render health text
    std::stringstream ss;
    ss << "HEALTH: " << health_ << " / " << max_health_;
    text_renderer_->RenderText(ss.str(), bar_x, bar_y - 30.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void EnhancedHUD::RenderScore() {
    float x = window_width_ - 300.0f;
    float y = window_height_ - 30.0f;

    std::stringstream ss;
    ss << "SCORE: " << score_;
    text_renderer_->RenderText(ss.str(), x, y, 1.0f, glm::vec3(0.0f, 1.0f, 1.0f));
}

void EnhancedHUD::RenderWaveInfo() {
    float x = window_width_ / 2.0f - 80.0f;
    float y = window_height_ - 30.0f;

    std::stringstream ss;
    ss << "WAVE " << wave_;
    text_renderer_->RenderText(ss.str(), x, y, 1.2f, glm::vec3(1.0f, 0.5f, 0.0f));
}

void EnhancedHUD::RenderAmmo() {
    float x = 20.0f;
    float y = window_height_ - 100.0f;

    // Laser ammo
    std::stringstream ss1;
    ss1 << "LASER: ";
    if (laser_ammo_ < 999) {
        ss1 << laser_ammo_;
    } else {
        ss1 << "∞";
    }
    text_renderer_->RenderText(ss1.str(), x, y, 0.7f, glm::vec3(0.0f, 1.0f, 1.0f));

    // Missile ammo
    std::stringstream ss2;
    ss2 << "MISSILE: ";
    if (missile_ammo_ < 999) {
        ss2 << missile_ammo_;
    } else {
        ss2 << "∞";
    }
    text_renderer_->RenderText(ss2.str(), x, y - 30.0f, 0.7f, glm::vec3(1.0f, 0.5f, 0.0f));
}

void EnhancedHUD::RenderComboMultiplier() {
    if (combo_multiplier_ > 1) {
        float x = window_width_ / 2.0f - 100.0f;
        float y = window_height_ - 100.0f;

        // Pulse effect for combo
        float pulse = (std::sin(game_time_ * 5.0f) + 1.0f) * 0.5f;
        float scale = 1.5f + pulse * 0.3f;

        std::stringstream ss;
        ss << "x" << combo_multiplier_ << " COMBO!";
        text_renderer_->RenderText(ss.str(), x, y, scale,
                                   glm::vec3(1.0f, 1.0f, 0.0f + pulse * 0.5f));
    }
}

void EnhancedHUD::RenderGameTime() {
    float x = window_width_ - 200.0f;
    float y = 30.0f;

    int minutes = static_cast<int>(game_time_) / 60;
    int seconds = static_cast<int>(game_time_) % 60;

    std::stringstream ss;
    ss << "TIME: " << std::setfill('0') << std::setw(2) << minutes
       << ":" << std::setfill('0') << std::setw(2) << seconds;
    text_renderer_->RenderText(ss.str(), x, y, 0.7f, glm::vec3(0.7f, 0.7f, 0.7f));
}

void EnhancedHUD::RenderCrosshair() {
    float center_x = window_width_ / 2.0f;
    float center_y = window_height_ / 2.0f;
    float size = 10.0f;

    // Simple crosshair using text (could be replaced with actual geometry)
    text_renderer_->RenderText("+", center_x - 8.0f, center_y - 8.0f, 1.0f,
                               glm::vec3(0.0f, 1.0f, 0.0f));
}

void EnhancedHUD::RenderScorePopups() {
    for (const auto& popup : score_popups_) {
        float alpha = 1.0f - (popup.lifetime / popup.max_lifetime);
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.0f) * alpha;

        std::stringstream ss;
        ss << "+" << popup.score;
        text_renderer_->RenderText(ss.str(), popup.x, popup.y, 1.2f, color);
    }
}

void EnhancedHUD::RenderDamageFlash() {
    if (damage_flash_timer_ > 0.0f) {
        // Render red vignette effect (simplified - just render text indicator)
        float alpha = damage_flash_timer_ / 0.5f;
        text_renderer_->RenderText("!", 50.0f, window_height_ / 2.0f, 3.0f,
                                   glm::vec3(1.0f, 0.0f, 0.0f) * alpha);
    }
}

void EnhancedHUD::RenderBar(float x, float y, float width, float height,
                             float fill_percentage, const glm::vec3& color) {
    float fill_width = width * fill_percentage;

    float vertices[] = {
        x, y,
        x + fill_width, y,
        x + fill_width, y + height,

        x, y,
        x + fill_width, y + height,
        x, y + height
    };

    glUseProgram(bar_shader_program_);

    // Set projection matrix
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(window_width_),
                                      0.0f, static_cast<float>(window_height_));
    glUniformMatrix4fv(glGetUniformLocation(bar_shader_program_, "projection"),
                       1, GL_FALSE, &projection[0][0]);

    // Set color
    glUniform3f(glGetUniformLocation(bar_shader_program_, "color"),
                color.x, color.y, color.z);

    glBindVertexArray(bar_VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, bar_VBO_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}
