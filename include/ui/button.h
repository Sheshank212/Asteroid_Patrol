#ifndef BUTTON_H
#define BUTTON_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <functional>
#include <memory>

class TextRenderer;

/**
 * Button - Interactive UI element for menus
 * Supports hover effects, click callbacks, and visual feedback
 */
class Button {
public:
    Button(const std::string& text, float x, float y, float width, float height);
    ~Button();

    // Update button state (check for hover)
    void Update(double mouse_x, double mouse_y);

    // Render the button
    void Render(TextRenderer* text_renderer, GLuint shader_program);

    // Check if button was clicked
    bool IsClicked(double mouse_x, double mouse_y);

    // Set callback function for when button is clicked
    void SetCallback(std::function<void()> callback);

    // Execute the button's callback
    void Execute();

    // Getters
    bool IsHovered() const { return is_hovered_; }
    bool IsPressed() const { return is_pressed_; }

    // Setters
    void SetPressed(bool pressed) { is_pressed_ = pressed; }
    void SetEnabled(bool enabled) { enabled_ = enabled; }
    bool IsEnabled() const { return enabled_; }

private:
    std::string text_;
    float x_, y_;              // Position (center)
    float width_, height_;      // Dimensions
    bool is_hovered_;
    bool is_pressed_;
    bool enabled_;

    glm::vec3 normal_color_;
    glm::vec3 hover_color_;
    glm::vec3 pressed_color_;
    glm::vec3 disabled_color_;

    std::function<void()> callback_;

    // Quad rendering for button background
    GLuint VAO_, VBO_;
    void InitializeQuad();
    void RenderQuad(GLuint shader_program, const glm::vec3& color);
};

#endif // BUTTON_H
