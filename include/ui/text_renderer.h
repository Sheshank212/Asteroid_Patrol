#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>
#include <string>

// Character holding all state information relevant to a single character
struct Character {
    GLuint textureID;   // ID handle of the glyph texture
    glm::ivec2 size;    // Size of glyph
    glm::ivec2 bearing; // Offset from baseline to left/top of glyph
    GLuint advance;     // Horizontal offset to advance to next glyph
};

/**
 * TextRenderer - Renders text using bitmap fonts
 * Uses a pre-generated bitmap font atlas for performance
 * Supports different font sizes and colors
 */
class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    // Initialize the text renderer with shader program
    bool Initialize(GLuint shader_program);

    // Render text at specified position with color and scale
    void RenderText(const std::string& text, float x, float y, float scale,
                    const glm::vec3& color);

    // Set the projection matrix for text rendering (orthographic)
    void SetProjection(const glm::mat4& projection);

    // Load a bitmap font (simplified version without FreeType)
    bool LoadBitmapFont();

private:
    GLuint VAO, VBO;
    GLuint shader_program_;
    glm::mat4 projection_;
    std::map<char, Character> characters_;

    // Create a simple bitmap font atlas
    void CreateSimpleFontAtlas();
};

#endif // TEXT_RENDERER_H
