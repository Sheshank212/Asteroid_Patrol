/* Asteroid Patrol - Final Project
 * Submitted by: Sheshank, Prajuvin, Balaji
 * Submitted to: Professor Rosa Azami
 * Course: COSC3406 - Advanced Game Design and Development
 *
 * 3D space survival game where players pilot a spaceship through an asteroid field.
 * Core features: Physics-based ship controls, dual camera modes, laser/missile weapons,
 * collision detection, scene graph hierarchy, particle effects, HUD, game states.
 *
 * Controls:
 *   W/S         - Forward/Backward
 *   A/D         - Left/Right strafe
 *   Arrow Keys  - Rotate ship
 *   SPACE       - Fire laser (in menu: start game)
 *   M           - Fire missile
 *   C           - Toggle camera view
 *   P           - Pause/Resume
 *   R           - Restart (when game over)
 *   Q           - Quit
 */

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Include our custom headers
#include "model.h"
#include "scene_node.h"
#include "ship.h"
#include "camera.h"
#include "laser.h"
#include "missile.h"
#include "asteroid.h"
#include "geometry.h"
#include "game_state.h"
#include "hud.h"
#include "starfield.h"
#include "particle_system.h"

// UI System
#include "ui/text_renderer.h"
#include "ui/button.h"
#include "ui/menu_manager.h"
#include "ui/enhanced_hud.h"

// Window settings
const std::string window_title_g = "Asteroid Patrol - Final Project";
const unsigned int window_width_g = 1280;
const unsigned int window_height_g = 720;
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.05);

// Camera settings
float camera_near_clip_distance_g = 0.1f;
float camera_far_clip_distance_g = 1000.0f;
float camera_fov_g = 60.0f;

// Shaders
const char *source_vp = "#version 130\n\
\n\
in vec3 vertex;\n\
in vec3 normal;\n\
in vec3 color;\n\
\n\
uniform mat4 world_mat;\n\
uniform mat4 view_mat;\n\
uniform mat4 projection_mat;\n\
uniform mat4 normal_mat;\n\
\n\
out vec4 color_interp;\n\
out vec3 normal_interp;\n\
out vec3 position_interp;\n\
\n\
void main()\n\
{\n\
    vec4 position = world_mat * vec4(vertex, 1.0);\n\
    gl_Position = projection_mat * view_mat * position;\n\
    \n\
    position_interp = position.xyz;\n\
    normal_interp = (normal_mat * vec4(normal, 0.0)).xyz;\n\
    color_interp = vec4(color, 1.0);\n\
}";

const char *source_fp = "#version 130\n\
\n\
in vec4 color_interp;\n\
in vec3 normal_interp;\n\
in vec3 position_interp;\n\
\n\
out vec4 FragColor;\n\
\n\
void main()\n\
{\n\
    vec3 light_pos = vec3(0.0, 100.0, 100.0);\n\
    vec3 light_dir = normalize(light_pos - position_interp);\n\
    vec3 normal = normalize(normal_interp);\n\
    \n\
    float diffuse = max(dot(normal, light_dir), 0.0);\n\
    vec3 ambient = 0.4 * color_interp.rgb;\n\
    vec3 lighting = ambient + diffuse * color_interp.rgb * 0.6;\n\
    \n\
    FragColor = vec4(lighting, color_interp.a);\n\
}";

// Global game objects
Ship* g_ship = nullptr;
Camera* g_camera = nullptr;
SceneNode* g_root = nullptr;
SceneNode* g_cannon_root = nullptr;
std::vector<Laser*> g_lasers;
std::vector<Missile*> g_missiles;
std::vector<Asteroid*> g_asteroids;
GLuint g_program = 0;
GLuint g_particle_program = 0;  // Particle shader program
glm::mat4 g_projection_matrix;
double g_last_time = 0.0;

// New game systems
GameManager* g_game_manager = nullptr;
HUD* g_hud = nullptr;
Starfield* g_starfield = nullptr;
ParticleSystem* g_particle_system = nullptr;

// UI System
MenuManager* g_menu_manager = nullptr;
EnhancedHUD* g_enhanced_hud = nullptr;

// Mouse state
double g_mouse_x = 0.0;
double g_mouse_y = 0.0;

// Forward declaration
void InitializeScene();

// Helper function to load shader source from file
std::string LoadShaderFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open shader file: " + filepath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Mouse position callback
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    g_mouse_x = xpos;
    g_mouse_y = window_height_g - ypos; // Flip Y coordinate for OpenGL

    if (g_menu_manager) {
        g_menu_manager->Update(g_mouse_x, g_mouse_y);
    }
}

// Mouse button callback
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (g_menu_manager) {
            g_menu_manager->HandleClick(g_mouse_x, g_mouse_y);
        }
    }
}

// Input handling
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Menu state - start game
    if (g_game_manager->current_state == GameState::MENU && key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        g_game_manager->StartGame();
        return;
    }

    // Game over state - restart
    if (g_game_manager->current_state == GameState::GAME_OVER && key == GLFW_KEY_R && action == GLFW_PRESS) {
        g_asteroids.clear();
        delete g_root;
        InitializeScene();
        g_game_manager->StartGame();
        return;
    }

    // Escape key to pause or return to menu
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        if (g_game_manager->current_state == GameState::PLAYING) {
            g_game_manager->PauseGame();
            if (g_menu_manager) {
                g_menu_manager->SetCurrentMenu(MenuState::PAUSED);
            }
        } else if (g_menu_manager) {
            // Handle ESC in sub-menus - go back to main menu
            MenuState current = g_menu_manager->GetCurrentMenu();
            if (current == MenuState::INSTRUCTIONS ||
                current == MenuState::SETTINGS ||
                current == MenuState::HIGH_SCORES) {
                g_menu_manager->SetCurrentMenu(MenuState::MAIN_MENU);
            }
        }
        return;
    }

    // Pause/resume
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        if (g_game_manager->current_state == GameState::PLAYING) {
            g_game_manager->PauseGame();
            if (g_menu_manager) {
                g_menu_manager->SetCurrentMenu(MenuState::PAUSED);
            }
        } else if (g_game_manager->current_state == GameState::PAUSED) {
            g_game_manager->ResumeGame();
            if (g_menu_manager) {
                g_menu_manager->SetCurrentMenu(MenuState::NONE);
            }
        }
        return;
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        g_camera->ToggleView();
    }

    // Only allow ship controls during gameplay
    if (g_game_manager->current_state != GameState::PLAYING) return;

    // Ship movement
    if (key == GLFW_KEY_W) g_ship->moving_forward = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_S) g_ship->moving_backward = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_A) g_ship->moving_left = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_D) g_ship->moving_right = (action != GLFW_RELEASE);

    // Fire laser
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        Laser* laser = nullptr;
        for (auto l : g_lasers) {
            if (!l->active) {
                laser = l;
                break;
            }
        }
        if (!laser) {
            laser = new Laser();
            laser->model = CreateCube(1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            g_lasers.push_back(laser);
            g_root->AddChild(laser);
        }

        glm::vec3 fire_pos = g_ship->position + g_ship->GetForward() * 2.0f;
        laser->Fire(fire_pos, g_ship->orientation);
    }

    // Fire missile
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        Missile* missile = nullptr;
        for (auto m : g_missiles) {
            if (!m->active) {
                missile = m;
                break;
            }
        }
        if (!missile) {
            missile = new Missile();
            missile->model = CreateCylinder(0.5f, 1.0f, 8, glm::vec3(1.0f, 1.0f, 0.0f));
            g_missiles.push_back(missile);
            g_root->AddChild(missile);
        }

        glm::vec3 fire_pos = g_ship->position + g_ship->GetForward() * 2.0f;
        missile->Fire(fire_pos, g_ship->orientation);
    }

    // Ship rotation
    if (key == GLFW_KEY_UP) {
        glm::quat rotation = glm::angleAxis(glm::radians(2.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        g_ship->orientation = rotation * g_ship->orientation;
    }
    if (key == GLFW_KEY_DOWN) {
        glm::quat rotation = glm::angleAxis(glm::radians(-2.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        g_ship->orientation = rotation * g_ship->orientation;
    }
    if (key == GLFW_KEY_LEFT) {
        glm::quat rotation = glm::angleAxis(glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        g_ship->orientation = rotation * g_ship->orientation;
    }
    if (key == GLFW_KEY_RIGHT) {
        glm::quat rotation = glm::angleAxis(glm::radians(-2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        g_ship->orientation = rotation * g_ship->orientation;
    }
}

void ResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    float aspect = (float)width / (float)height;
    g_projection_matrix = glm::perspective(glm::radians(camera_fov_g), aspect, camera_near_clip_distance_g, camera_far_clip_distance_g);
}

// Game logic update
void UpdateGame(float delta_time) {
    if (g_game_manager->current_state != GameState::PLAYING) {
        return;
    }

    g_game_manager->game_time += delta_time;
    g_ship->Update(delta_time);
    g_camera->UpdateCameraPosition(g_ship);
    g_particle_system->Update(static_cast<float>(glfwGetTime()));

    // Update lasers and check collisions
    for (auto laser : g_lasers) {
        if (laser->active) {
            laser->Update(delta_time);
            for (auto asteroid : g_asteroids) {
                if (asteroid->visible && !asteroid->hit) {
                    if (asteroid->CheckRayIntersection(laser->GetRayStart(), laser->GetRayDirection())) {
                        asteroid->hit = true;
                        asteroid->visible = false;
                        laser->active = false;
                        laser->visible = false;
                        g_game_manager->AddScore(100);
                        g_particle_system->SpawnExplosion(asteroid->position, glm::vec3(1.0f, 0.5f, 0.0f));
                        break;
                    }
                }
            }
        }
    }

    // Update missiles and check collisions
    for (auto missile : g_missiles) {
        if (missile->active) {
            missile->Update(delta_time);
            for (auto asteroid : g_asteroids) {
                if (asteroid->visible && !asteroid->hit) {
                    if (asteroid->CheckRayIntersection(missile->GetRayStart(), missile->GetRayDirection())) {
                        asteroid->hit = true;
                        asteroid->visible = false;
                        missile->active = false;
                        missile->visible = false;
                        g_game_manager->AddScore(150);
                        g_particle_system->SpawnExplosion(asteroid->position, glm::vec3(1.0f, 0.7f, 0.0f));
                        break;
                    }
                }
            }
        }
    }

    // Update asteroids and check ship collision
    for (auto asteroid : g_asteroids) {
        if (asteroid->visible) {
            glm::quat rotation = glm::angleAxis(delta_time * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
            asteroid->orientation = asteroid->orientation * rotation;

            // Check ship collision
            float ship_radius = 1.5f;
            if (asteroid->CheckMissileIntersection(g_ship->position, ship_radius)) {
                asteroid->hit = true;
                asteroid->visible = false;
                g_game_manager->TakeDamage(20);
                g_particle_system->SpawnExplosion(asteroid->position, glm::vec3(1.0f, 0.3f, 0.0f));
            }
        }
    }

    // Update cannon animation
    if (g_cannon_root) {
        double current_time = glfwGetTime();
        glm::quat cannon_rotation = glm::angleAxis((float)current_time * 0.3f, glm::vec3(0.0f, 1.0f, 0.0f));
        g_cannon_root->orientation = cannon_rotation;
    }
}

// Initialize scene
void InitializeScene() {
    g_root = new SceneNode("Root");

    // Create ship
    g_ship = new Ship();
    g_ship->position = glm::vec3(0.0f, 0.0f, 0.0f);
    g_root->AddChild(g_ship);

    // Ship body
    SceneNode* ship_body = new SceneNode("ShipBody");
    ship_body->model = CreateCube(1.0f, glm::vec3(0.2f, 0.5f, 0.9f));
    ship_body->scale = glm::vec3(1.0f, 0.8f, 2.5f);
    g_ship->AddChild(ship_body);

    // Ship nose
    SceneNode* ship_nose = new SceneNode("ShipNose");
    ship_nose->model = CreateCube(0.6f, glm::vec3(0.3f, 0.8f, 1.0f));
    ship_nose->position = glm::vec3(0.0f, 0.2f, -1.5f);
    ship_nose->scale = glm::vec3(0.7f, 0.7f, 0.6f);
    g_ship->AddChild(ship_nose);

    // Wings
    SceneNode* left_wing = new SceneNode("LeftWing");
    left_wing->model = CreateCube(0.5f, glm::vec3(0.4f, 0.6f, 0.8f));
    left_wing->position = glm::vec3(-1.2f, 0.0f, 0.3f);
    left_wing->scale = glm::vec3(2.0f, 0.2f, 1.5f);
    g_ship->AddChild(left_wing);

    SceneNode* right_wing = new SceneNode("RightWing");
    right_wing->model = CreateCube(0.5f, glm::vec3(0.4f, 0.6f, 0.8f));
    right_wing->position = glm::vec3(1.2f, 0.0f, 0.3f);
    right_wing->scale = glm::vec3(2.0f, 0.2f, 1.5f);
    g_ship->AddChild(right_wing);

    // Engines
    SceneNode* engine_left = new SceneNode("EngineLeft");
    engine_left->model = CreateCube(0.3f, glm::vec3(1.0f, 0.5f, 0.0f));
    engine_left->position = glm::vec3(-0.5f, 0.0f, 1.3f);
    engine_left->scale = glm::vec3(0.4f, 0.4f, 0.4f);
    g_ship->AddChild(engine_left);

    SceneNode* engine_right = new SceneNode("EngineRight");
    engine_right->model = CreateCube(0.3f, glm::vec3(1.0f, 0.5f, 0.0f));
    engine_right->position = glm::vec3(0.5f, 0.0f, 1.3f);
    engine_right->scale = glm::vec3(0.4f, 0.4f, 0.4f);
    g_ship->AddChild(engine_right);

    // Create camera
    g_camera = new Camera();
    g_ship->AddChild(g_camera);

    // Create asteroids
    for (int i = 0; i < 15; i++) {
        Asteroid* asteroid = new Asteroid();
        float angle = (float)i / 15.0f * 2.0f * glm::pi<float>();
        float distance = 20.0f + (i % 3) * 15.0f;
        asteroid->position = glm::vec3(cos(angle) * distance, (rand() % 20 - 10) * 0.5f, sin(angle) * distance);
        asteroid->scale = glm::vec3(1.5f);

        float hue = (float)i / 15.0f * 360.0f;
        glm::vec3 ast_color = HSVtoRGB(hue, 0.8f, 0.9f);
        asteroid->model = CreateSphere(1.0f, 12, 24, ast_color);
        g_asteroids.push_back(asteroid);
        g_root->AddChild(asteroid);
    }

    // Create cannon
    g_cannon_root = new SceneNode("CannonBase");
    g_cannon_root->position = glm::vec3(-30.0f, 0.0f, 0.0f);

    SceneNode* cannon_base = new SceneNode("CannonBaseCylinder");
    cannon_base->model = CreateCylinder(2.0f, 0.8f, 32, glm::vec3(0.5f, 0.5f, 0.5f));
    g_cannon_root->AddChild(cannon_base);

    SceneNode* cannon_barrel = new SceneNode("CannonBarrel");
    cannon_barrel->position = glm::vec3(0.0f, 1.0f, 0.0f);
    cannon_barrel->orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cannon_barrel->model = CreateCylinder(0.6f, 4.0f, 16, glm::vec3(0.3f, 0.3f, 0.3f));
    g_cannon_root->AddChild(cannon_barrel);

    g_root->AddChild(g_cannon_root);

    // Initialize game systems (only once)
    if (!g_game_manager) {
        g_game_manager = new GameManager();
        g_hud = new HUD();
        g_starfield = new Starfield(1000);
        g_particle_system = new ParticleSystem(500);
    }
}

int main(void) {
    try {
        // Initialize GLFW
        if (!glfwInit()) {
            throw(std::runtime_error("Could not initialize GLFW"));
        }

        // Create window
        GLFWwindow* window = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
        if (!window) {
            glfwTerminate();
            throw(std::runtime_error("Could not create window"));
        }
        glfwMakeContextCurrent(window);

        // Initialize GLEW
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            throw(std::runtime_error(std::string("Could not initialize GLEW: ") + (const char*)glewGetErrorString(err)));
        }

        // OpenGL setup
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Create shaders
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &source_vp, NULL);
        glCompileShader(vs);

        GLint status;
        glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            char buffer[512];
            glGetShaderInfoLog(vs, 512, NULL, buffer);
            throw(std::runtime_error(std::string("Error compiling vertex shader: ") + buffer));
        }

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &source_fp, NULL);
        glCompileShader(fs);

        glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            char buffer[512];
            glGetShaderInfoLog(fs, 512, NULL, buffer);
            throw(std::runtime_error(std::string("Error compiling fragment shader: ") + buffer));
        }

        g_program = glCreateProgram();
        glAttachShader(g_program, vs);
        glAttachShader(g_program, fs);
        glLinkProgram(g_program);

        glGetProgramiv(g_program, GL_LINK_STATUS, &status);
        if (status != GL_TRUE) {
            char buffer[512];
            glGetProgramInfoLog(g_program, 512, NULL, buffer);
            throw(std::runtime_error(std::string("Error linking shaders: ") + buffer));
        }

        glDeleteShader(vs);
        glDeleteShader(fs);

        // Create particle shader program with geometry shader
        // Particle rendering based on Prof. Azami's ParticleDemo
        std::cout << "Loading particle shaders..." << std::endl;

        std::string particle_vp_source = LoadShaderFile("shaders/particle_vp.glsl");
        std::string particle_gp_source = LoadShaderFile("shaders/particle_gp.glsl");
        std::string particle_fp_source = LoadShaderFile("shaders/particle_fp.glsl");

        const char* particle_vp_c = particle_vp_source.c_str();
        const char* particle_gp_c = particle_gp_source.c_str();
        const char* particle_fp_c = particle_fp_source.c_str();

        GLuint particle_vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(particle_vs, 1, &particle_vp_c, NULL);
        glCompileShader(particle_vs);

        glGetShaderiv(particle_vs, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            char buffer[512];
            glGetShaderInfoLog(particle_vs, 512, NULL, buffer);
            throw(std::runtime_error(std::string("Error compiling particle vertex shader: ") + buffer));
        }

        GLuint particle_gs = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(particle_gs, 1, &particle_gp_c, NULL);
        glCompileShader(particle_gs);

        glGetShaderiv(particle_gs, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            char buffer[512];
            glGetShaderInfoLog(particle_gs, 512, NULL, buffer);
            throw(std::runtime_error(std::string("Error compiling particle geometry shader: ") + buffer));
        }

        GLuint particle_fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(particle_fs, 1, &particle_fp_c, NULL);
        glCompileShader(particle_fs);

        glGetShaderiv(particle_fs, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            char buffer[512];
            glGetShaderInfoLog(particle_fs, 512, NULL, buffer);
            throw(std::runtime_error(std::string("Error compiling particle fragment shader: ") + buffer));
        }

        g_particle_program = glCreateProgram();
        glAttachShader(g_particle_program, particle_vs);
        glAttachShader(g_particle_program, particle_gs);
        glAttachShader(g_particle_program, particle_fs);
        glLinkProgram(g_particle_program);

        glGetProgramiv(g_particle_program, GL_LINK_STATUS, &status);
        if (status != GL_TRUE) {
            char buffer[512];
            glGetProgramInfoLog(g_particle_program, 512, NULL, buffer);
            throw(std::runtime_error(std::string("Error linking particle shaders: ") + buffer));
        }

        glDeleteShader(particle_vs);
        glDeleteShader(particle_gs);
        glDeleteShader(particle_fs);

        std::cout << "Particle shaders loaded successfully!" << std::endl;

        // Set up projection
        float aspect = (float)window_width_g / (float)window_height_g;
        g_projection_matrix = glm::perspective(glm::radians(camera_fov_g), aspect, camera_near_clip_distance_g, camera_far_clip_distance_g);

        // Set callbacks
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetFramebufferSizeCallback(window, ResizeCallback);
        glfwSetCursorPosCallback(window, CursorPosCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);

        // Initialize scene
        InitializeScene();

        // Initialize Menu System
        g_menu_manager = new MenuManager(window_width_g, window_height_g);
        if (!g_menu_manager->Initialize()) {
            std::cerr << "Failed to initialize menu system" << std::endl;
        }

        // Set up menu callbacks
        g_menu_manager->SetStartGameCallback([&]() {
            g_game_manager->StartGame();
            g_menu_manager->SetCurrentMenu(MenuState::NONE);
        });

        g_menu_manager->SetQuitGameCallback([&window]() {
            glfwSetWindowShouldClose(window, true);
        });

        g_menu_manager->SetResumeGameCallback([&]() {
            g_game_manager->ResumeGame();
            g_menu_manager->SetCurrentMenu(MenuState::NONE);
        });

        g_menu_manager->SetRestartGameCallback([&]() {
            g_asteroids.clear();
            delete g_root;
            InitializeScene();
            g_game_manager->StartGame();
            g_menu_manager->SetCurrentMenu(MenuState::NONE);
        });

        // Set initial menu state
        g_menu_manager->SetCurrentMenu(MenuState::MAIN_MENU);

        // Initialize Enhanced HUD
        g_enhanced_hud = new EnhancedHUD(window_width_g, window_height_g);
        if (!g_enhanced_hud->Initialize(g_menu_manager->GetTextRenderer())) {
            std::cerr << "Failed to initialize enhanced HUD" << std::endl;
        }

        // Initialize particle system with particle shader
        g_particle_system->Initialize(g_particle_program);

        std::cout << "\n=== ASTEROID PATROL - Final Project ===" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  W/S        - Forward/Backward" << std::endl;
        std::cout << "  A/D        - Left/Right strafe" << std::endl;
        std::cout << "  Arrow Keys - Rotate ship" << std::endl;
        std::cout << "  SPACE      - Fire laser (in menu: start game)" << std::endl;
        std::cout << "  M          - Fire missile" << std::endl;
        std::cout << "  C          - Toggle camera" << std::endl;
        std::cout << "  P          - Pause/Resume" << std::endl;
        std::cout << "  R          - Restart (when game over)" << std::endl;
        std::cout << "  Q          - Quit" << std::endl;
        std::cout << "\nObjective: Destroy asteroids! Avoid collisions!" << std::endl;
        std::cout << "========================================\n" << std::endl;

        // Show menu
        g_hud->RenderMenu();

        g_last_time = glfwGetTime();

        // Main loop
        while (!glfwWindowShouldClose(window)) {
            double current_time = glfwGetTime();
            float delta_time = (float)(current_time - g_last_time);
            g_last_time = current_time;

            glClearColor(viewport_background_color_g[0], viewport_background_color_g[1], viewport_background_color_g[2], 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            UpdateGame(delta_time);

            glUseProgram(g_program);

            glm::mat4 view_matrix = g_camera->GetViewMatrix();
            GLint view_mat = glGetUniformLocation(g_program, "view_mat");
            glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view_matrix));

            GLint projection_mat = glGetUniformLocation(g_program, "projection_mat");
            glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(g_projection_matrix));

            // Render starfield first
            g_starfield->Render(g_program);

            // Hide ship during menu, otherwise show based on camera mode
            if (g_game_manager->current_state == GameState::MENU) {
                g_ship->visible = false;
            } else {
                g_ship->visible = !g_camera->is_first_person;
            }

            // Render scene
            g_root->Draw(g_program);

            // Render particles
            g_particle_system->Render(static_cast<float>(current_time), view_matrix, g_projection_matrix);

            // Handle different game states
            switch (g_game_manager->current_state) {
                case GameState::MENU:
                    if (g_menu_manager && g_menu_manager->GetCurrentMenu() != MenuState::NONE) {
                        g_menu_manager->Render();
                    }
                    break;
                case GameState::PLAYING:
                    if (g_enhanced_hud) {
                        g_enhanced_hud->UpdatePopups(delta_time);
                        g_enhanced_hud->SetHealth(g_game_manager->health);
                        g_enhanced_hud->SetMaxHealth(g_game_manager->max_health);
                        g_enhanced_hud->SetScore(g_game_manager->score);
                        g_enhanced_hud->SetWave(1);  // Clean version doesn't have wave system
                        g_enhanced_hud->SetGameTime(g_game_manager->game_time);
                        g_enhanced_hud->Render();
                    }
                    break;
                case GameState::PAUSED:
                    if (g_enhanced_hud) {
                        g_enhanced_hud->Render();
                    }
                    if (g_menu_manager) {
                        g_menu_manager->Render();
                    }
                    break;
                case GameState::GAME_OVER:
                    {
                        if (g_menu_manager) {
                            g_menu_manager->SetGameOverScore(g_game_manager->score, 1);
                            g_menu_manager->SetCurrentMenu(MenuState::GAME_OVER);
                            g_menu_manager->Render();
                        }
                    }
                    break;
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // Cleanup
        delete g_root;
        delete g_game_manager;
        delete g_hud;
        delete g_starfield;
        delete g_particle_system;
        delete g_menu_manager;
        delete g_enhanced_hud;

        // Cleanup shader programs
        if (g_program != 0) {
            glDeleteProgram(g_program);
        }
        if (g_particle_program != 0) {
            glDeleteProgram(g_particle_program);
        }

        glfwTerminate();
    }
    catch (std::exception &e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
