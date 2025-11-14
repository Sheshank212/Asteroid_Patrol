#include "asteroid.h"

Asteroid::Asteroid() : SceneNode("Asteroid") {
    radius = 1.5f;
    hit = false;
}

// Ray-sphere intersection for collision detection
bool Asteroid::CheckRayIntersection(glm::vec3 ray_origin, glm::vec3 ray_direction) {
    // Ray-sphere intersection formula
    // Reference: Real-Time Rendering, Chapter on Intersection Tests

    glm::vec3 sphere_center = position;
    float sphere_radius = radius * scale.x; // Account for scaling

    glm::vec3 oc = ray_origin - sphere_center;
    float a = glm::dot(ray_direction, ray_direction);
    float b = 2.0f * glm::dot(oc, ray_direction);
    float c = glm::dot(oc, oc) - sphere_radius * sphere_radius;
    float discriminant = b * b - 4 * a * c;

    return discriminant >= 0.0f;
}

// Check collision with missile (sphere-sphere)
bool Asteroid::CheckMissileIntersection(glm::vec3 missile_pos, float missile_radius) {
    float distance = glm::length(position - missile_pos);
    float combined_radius = radius * scale.x + missile_radius;
    return distance < combined_radius;
}
