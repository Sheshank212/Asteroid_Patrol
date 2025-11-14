#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

// Explosion instance - tracks position and timing for each explosion
struct Explosion {
    glm::vec3 position;
    float start_time;       // When explosion started
    float duration;         // How long explosion lasts
    glm::vec3 color;        // Explosion color
    bool active;
};

// Particle system for explosions using shader-based animation
// Based on Prof. Azami's ParticleDemo with sphere particle rendering
class ParticleSystem {
public:
    ParticleSystem(int num_particles = 5000, int max_explosions = 50);
    ~ParticleSystem();

    // Initialize OpenGL resources and create particle geometry
    void Initialize(GLuint particle_shader_program);

    // Spawn a new explosion at the given position
    void SpawnExplosion(const glm::vec3& position, const glm::vec3& color = glm::vec3(1.0f, 0.6f, 0.0f));

    // Update explosion states (remove expired ones)
    void Update(float current_time);

    // Render all active explosions
    void Render(float current_time, const glm::mat4& view_mat, const glm::mat4& projection_mat);

    // Cleanup OpenGL resources
    void Cleanup();

private:
    // OpenGL resources
    GLuint vao;                // Vertex Array Object
    GLuint vbo;                // Vertex Buffer Object
    GLuint shader_program;     // Particle shader program
    int num_particles;         // Number of particles per explosion

    // Explosion tracking
    std::vector<Explosion> explosions;
    int max_explosions;

    // Helper function to create sphere particle geometry
    // Adapted from Prof. Azami's ResourceManager::CreateSphereParticles
    void CreateSphereParticles();

    // Get an inactive explosion slot
    Explosion* GetInactiveExplosion();
};

#endif // PARTICLE_SYSTEM_H
