#include "particle_system.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

// Constructor
ParticleSystem::ParticleSystem(int num_particles, int max_explosions)
    : num_particles(num_particles), max_explosions(max_explosions),
      vao(0), vbo(0), shader_program(0) {

    // Initialize explosion pool
    explosions.resize(max_explosions);
    for (auto& explosion : explosions) {
        explosion.active = false;
    }

    // Seed random number generator
    srand(static_cast<unsigned int>(time(nullptr)) + 54321);
}

// Destructor
ParticleSystem::~ParticleSystem() {
    Cleanup();
}

// Initialize OpenGL resources
void ParticleSystem::Initialize(GLuint particle_shader_program) {
    this->shader_program = particle_shader_program;

    // Create particle geometry
    CreateSphereParticles();

    std::cout << "ParticleSystem initialized with " << num_particles << " particles per explosion" << std::endl;
}

// Create sphere particle geometry
// Adapted from Prof. Azami's ResourceManager::CreateSphereParticles
void ParticleSystem::CreateSphereParticles() {
    // Create a set of points which will be the particles
    // This is similar to drawing a sphere: we will sample points on a sphere,
    // but will allow them to also deviate a bit from the sphere along the normal

    // Number of attributes per particle: position (3), normal (3), color (3)
    const int particle_att = 9;

    // Allocate memory for particle data
    GLfloat* particle_data = new GLfloat[num_particles * particle_att];

    float trad = 0.2f;      // Starting point of particles along normal
    float maxspray = 0.5f;  // How much particles deviate from sphere
    float u, v, w, theta, phi, spray;

    for (int i = 0; i < num_particles; i++) {
        // Get three random numbers
        u = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        w = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        // Use u to define angle theta along one direction of sphere
        theta = u * 2.0f * 3.14159265359f;

        // Use v to define angle phi along other direction of sphere
        phi = acos(2.0f * v - 1.0f);

        // Use w to define deviation from sphere surface
        spray = maxspray * pow(w, 1.0f / 3.0f);  // Cubic root of w

        // Define the normal and point based on theta, phi and spray
        glm::vec3 normal(
            spray * cos(theta) * sin(phi),
            spray * sin(theta) * sin(phi),
            spray * cos(phi)
        );

        glm::vec3 position(
            normal.x * trad,
            normal.y * trad,
            normal.z * trad
        );

        // Color gradient (for debug/variety)
        glm::vec3 color(
            i / static_cast<float>(num_particles),
            0.0f,
            1.0f - i / static_cast<float>(num_particles)
        );

        // Add vectors to data buffer
        particle_data[i * particle_att + 0] = position.x;
        particle_data[i * particle_att + 1] = position.y;
        particle_data[i * particle_att + 2] = position.z;

        particle_data[i * particle_att + 3] = normal.x;
        particle_data[i * particle_att + 4] = normal.y;
        particle_data[i * particle_att + 5] = normal.z;

        particle_data[i * particle_att + 6] = color.r;
        particle_data[i * particle_att + 7] = color.g;
        particle_data[i * particle_att + 8] = color.b;
    }

    // Create VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO and upload data
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 num_particles * particle_att * sizeof(GLfloat),
                 particle_data,
                 GL_STATIC_DRAW);

    // Set up vertex attributes
    // Position attribute (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                         particle_att * sizeof(GLfloat),
                         (void*)0);

    // Normal attribute (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                         particle_att * sizeof(GLfloat),
                         (void*)(3 * sizeof(GLfloat)));

    // Color attribute (location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                         particle_att * sizeof(GLfloat),
                         (void*)(6 * sizeof(GLfloat)));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Free CPU memory
    delete[] particle_data;
}

// Get an inactive explosion slot
Explosion* ParticleSystem::GetInactiveExplosion() {
    for (auto& explosion : explosions) {
        if (!explosion.active) {
            return &explosion;
        }
    }
    return nullptr;  // All slots used
}

// Spawn a new explosion
void ParticleSystem::SpawnExplosion(const glm::vec3& position, const glm::vec3& color) {
    Explosion* exp = GetInactiveExplosion();
    if (!exp) {
        std::cout << "Warning: No available explosion slots!" << std::endl;
        return;  // No available slots
    }

    exp->position = position;
    exp->start_time = static_cast<float>(glfwGetTime());
    exp->duration = 2.0f;  // Explosion lasts 2 seconds
    exp->color = color;
    exp->active = true;
}

// Update explosion states
void ParticleSystem::Update(float current_time) {
    for (auto& explosion : explosions) {
        if (!explosion.active) continue;

        // Check if explosion has expired
        float elapsed = current_time - explosion.start_time;
        if (elapsed >= explosion.duration) {
            explosion.active = false;
        }
    }
}

// Render all active explosions
void ParticleSystem::Render(float current_time, const glm::mat4& view_mat, const glm::mat4& projection_mat) {
    if (shader_program == 0) {
        std::cout << "Warning: Particle shader not initialized!" << std::endl;
        return;
    }

    // Use particle shader
    glUseProgram(shader_program);

    // Bind particle VAO
    glBindVertexArray(vao);

    // Enable blending for particle effects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth writing (but keep depth testing) so particles blend properly
    glDepthMask(GL_FALSE);

    // Set view and projection matrices (same for all explosions)
    GLint view_mat_loc = glGetUniformLocation(shader_program, "view_mat");
    GLint proj_mat_loc = glGetUniformLocation(shader_program, "projection_mat");

    if (view_mat_loc != -1) {
        glUniformMatrix4fv(view_mat_loc, 1, GL_FALSE, &view_mat[0][0]);
    }
    if (proj_mat_loc != -1) {
        glUniformMatrix4fv(proj_mat_loc, 1, GL_FALSE, &projection_mat[0][0]);
    }

    // Render each active explosion
    for (const auto& explosion : explosions) {
        if (!explosion.active) continue;

        // Calculate elapsed time for this explosion
        float elapsed = current_time - explosion.start_time;

        // Create world matrix (translation to explosion position)
        glm::mat4 world_mat = glm::translate(glm::mat4(1.0f), explosion.position);

        // Set uniforms for this explosion
        GLint world_mat_loc = glGetUniformLocation(shader_program, "world_mat");
        GLint timer_loc = glGetUniformLocation(shader_program, "timer");
        GLint normal_mat_loc = glGetUniformLocation(shader_program, "normal_mat");
        GLint color_loc = glGetUniformLocation(shader_program, "object_color");

        if (world_mat_loc != -1) {
            glUniformMatrix4fv(world_mat_loc, 1, GL_FALSE, &world_mat[0][0]);
        }

        if (timer_loc != -1) {
            glUniform1f(timer_loc, elapsed);
        }

        if (normal_mat_loc != -1) {
            // Normal matrix is transpose of inverse of world matrix
            glm::mat4 normal_mat = glm::transpose(glm::inverse(world_mat));
            glUniformMatrix4fv(normal_mat_loc, 1, GL_FALSE, &normal_mat[0][0]);
        }

        if (color_loc != -1) {
            glUniform3fv(color_loc, 1, &explosion.color[0]);
        }

        // Draw particles as points
        glDrawArrays(GL_POINTS, 0, num_particles);
    }

    // Restore OpenGL state
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glUseProgram(0);
}

// Cleanup OpenGL resources
void ParticleSystem::Cleanup() {
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
}
