#ifndef STARFIELD_H
#define STARFIELD_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

// Individual star data
struct Star {
    glm::vec3 position;
    float brightness;
    float size;
};

// Starfield background with procedural stars
class Starfield {
public:
    Starfield(int num_stars = 1000);
    ~Starfield();

    void Render(GLuint shader_program);

private:
    std::vector<Star> stars;
    GLuint vbo;
    GLuint vao;
    int star_count;

    void GenerateStars(int num_stars);
    void InitializeBuffers();
};

#endif // STARFIELD_H
