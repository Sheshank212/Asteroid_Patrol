#include "starfield.h"
#include <cstdlib>
#include <ctime>

Starfield::Starfield(int num_stars) : star_count(num_stars) {
    GenerateStars(num_stars);
    InitializeBuffers();
}

Starfield::~Starfield() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Starfield::GenerateStars(int num_stars) {
    srand(static_cast<unsigned int>(time(nullptr)));

    stars.clear();
    for (int i = 0; i < num_stars; i++) {
        Star star;

        // Random positions in a large cube around the origin
        star.position = glm::vec3(
            (rand() % 2000 - 1000) / 10.0f,  // -100 to 100
            (rand() % 2000 - 1000) / 10.0f,
            (rand() % 2000 - 1000) / 10.0f
        );

        // Random brightness
        star.brightness = 0.3f + (rand() % 700) / 1000.0f;  // 0.3 to 1.0

        // Random size
        star.size = 1.0f + (rand() % 200) / 100.0f;  // 1.0 to 3.0

        stars.push_back(star);
    }
}

void Starfield::InitializeBuffers() {
    // Create vertex data for point rendering
    std::vector<float> vertex_data;

    for (const auto& star : stars) {
        // Position (3 floats)
        vertex_data.push_back(star.position.x);
        vertex_data.push_back(star.position.y);
        vertex_data.push_back(star.position.z);

        // Normal (use as brightness - 3 floats)
        vertex_data.push_back(star.brightness);
        vertex_data.push_back(star.brightness);
        vertex_data.push_back(star.brightness);

        // Color (white - 3 floats)
        vertex_data.push_back(1.0f);
        vertex_data.push_back(1.0f);
        vertex_data.push_back(1.0f);
    }

    // Create VAO and VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute (location 1) - used for brightness
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Color attribute (location 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Starfield::Render(GLuint shader_program) {
    // Enable point size for star rendering
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SMOOTH);

    // Disable depth writing so stars appear in background
    glDepthMask(GL_FALSE);

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, star_count);
    glBindVertexArray(0);

    // Re-enable depth writing
    glDepthMask(GL_TRUE);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_PROGRAM_POINT_SIZE);
}
