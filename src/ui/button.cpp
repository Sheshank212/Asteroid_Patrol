#include "ui/button.h"
#include "ui/text_renderer.h"
#include <iostream>
#include <functional>

Button::Button(const std::string& text, float x, float y, float width, float height)
    : text_(text), x_(x), y_(y), width_(width), height_(height),
      is_hovered_(false), is_pressed_(false), enabled_(true),
      VAO_(0), VBO_(0), callback_(nullptr) {

    // Set default colors
    normal_color_ = glm::vec3(0.2f, 0.3f, 0.5f);    // Dark blue
    hover_color_ = glm::vec3(0.3f, 0.5f, 0.8f);     // Bright blue
    pressed_color_ = glm::vec3(0.1f, 0.2f, 0.4f);   // Darker blue
    disabled_color_ = glm::vec3(0.3f, 0.3f, 0.3f);  // Gray

    InitializeQuad();
}

Button::~Button() {
    if (VAO_) glDeleteVertexArrays(1, &VAO_);
    if (VBO_) glDeleteBuffers(1, &VBO_);
}

void Button::InitializeQuad() {
    // Create a simple quad for button background
    float half_width = width_ / 2.0f;
    float half_height = height_ / 2.0f;

    float vertices[] = {
        // Position
        x_ - half_width, y_ - half_height,
        x_ + half_width, y_ - half_height,
        x_ + half_width, y_ + half_height,

        x_ - half_width, y_ - half_height,
        x_ + half_width, y_ + half_height,
        x_ - half_width, y_ + half_height
    };

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Button::Update(double mouse_x, double mouse_y) {
    if (!enabled_) {
        is_hovered_ = false;
        return;
    }

    // Check if mouse is over button
    float half_width = width_ / 2.0f;
    float half_height = height_ / 2.0f;

    is_hovered_ = (mouse_x >= x_ - half_width && mouse_x <= x_ + half_width &&
                   mouse_y >= y_ - half_height && mouse_y <= y_ + half_height);
}

bool Button::IsClicked(double mouse_x, double mouse_y) {
    if (!enabled_) return false;

    float half_width = width_ / 2.0f;
    float half_height = height_ / 2.0f;

    return (mouse_x >= x_ - half_width && mouse_x <= x_ + half_width &&
            mouse_y >= y_ - half_height && mouse_y <= y_ + half_height);
}

void Button::SetCallback(std::function<void()> callback) {
    callback_ = callback;
}

void Button::Execute() {
    if (callback_ && enabled_) {
        callback_();
    }
}

void Button::RenderQuad(GLuint shader_program, const glm::vec3& color) {
    glUseProgram(shader_program);
    glUniform3f(glGetUniformLocation(shader_program, "color"), color.x, color.y, color.z);

    glBindVertexArray(VAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Button::Render(TextRenderer* text_renderer, GLuint shader_program) {
    if (!text_renderer) return;

    // Determine button color based on state
    glm::vec3 button_color;
    glm::vec3 text_color(1.0f, 1.0f, 1.0f); // White text

    if (!enabled_) {
        button_color = disabled_color_;
        text_color = glm::vec3(0.5f, 0.5f, 0.5f); // Gray text
    } else if (is_pressed_) {
        button_color = pressed_color_;
    } else if (is_hovered_) {
        button_color = hover_color_;
        text_color = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow text on hover
    } else {
        button_color = normal_color_;
    }

    // Render button background quad
    if (shader_program && VAO_) {
        RenderQuad(shader_program, button_color);
    }

    // Calculate text position (centered on button)
    float text_x = x_ - (text_.length() * 8.0f); // Approximate centering
    float text_y = y_ - 10.0f;

    // Render text
    text_renderer->RenderText(text_, text_x, text_y, 1.0f, text_color);
}
