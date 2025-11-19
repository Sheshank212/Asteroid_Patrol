#ifndef ENHANCED_HUD_H
#define ENHANCED_HUD_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <memory>
#include <vector>

class TextRenderer;

/**
 * EnhancedHUD - Modern OpenGL-based HUD system
 * Displays health bar, score, combo multiplier, ammo, wave info, etc.
 */
class EnhancedHUD {
public:
    EnhancedHUD(int window_width, int window_height);
    ~EnhancedHUD();

    // Initialize HUD resources
    bool Initialize(TextRenderer* text_renderer);

    // Update HUD data
    void SetHealth(int health) { health_ = health; }
    void SetMaxHealth(int max_health) { max_health_ = max_health; }
    void SetScore(int score) { score_ = score; }
    void SetWave(int wave) { wave_ = wave; }
    void SetComboMultiplier(int combo) { combo_multiplier_ = combo; }
    void SetLaserAmmo(int ammo) { laser_ammo_ = ammo; }
    void SetMissileAmmo(int ammo) { missile_ammo_ = ammo; }
    void SetGameTime(float time) { game_time_ = time; }

    // Render the HUD
    void Render();

    // Show score popup (when destroying asteroid)
    void ShowScorePopup(int score, float x, float y);

    // Update score popups (fade out over time)
    void UpdatePopups(float delta_time);

    // Trigger damage flash effect
    void TriggerDamageFlash() { damage_flash_timer_ = 0.5f; }

private:
    int window_width_, window_height_;
    TextRenderer* text_renderer_;

    // HUD data
    int health_;
    int max_health_;
    int score_;
    int wave_;
    int combo_multiplier_;
    int laser_ammo_;
    int missile_ammo_;
    float game_time_;

    // Visual effects
    float damage_flash_timer_;
    float low_health_pulse_;

    // Score popup system
    struct ScorePopup {
        int score;
        float x, y;
        float lifetime;
        float max_lifetime;
    };
    std::vector<ScorePopup> score_popups_;

    // Rendering functions
    void RenderHealthBar();
    void RenderScore();
    void RenderWaveInfo();
    void RenderAmmo();
    void RenderComboMultiplier();
    void RenderGameTime();
    void RenderCrosshair();
    void RenderScorePopups();
    void RenderDamageFlash();

    // Bar rendering
    GLuint bar_VAO_, bar_VBO_;
    GLuint bar_shader_program_;
    void InitializeBarRendering();
    void RenderBar(float x, float y, float width, float height,
                   float fill_percentage, const glm::vec3& color);
};

#endif // ENHANCED_HUD_H
