#include "ship.h"
#include <glm/gtc/matrix_transform.hpp>

Ship::Ship() : SceneNode("Ship") {
    velocity = glm::vec3(0.0f);
    acceleration = glm::vec3(0.0f);
    max_speed = 15.0f;
    acceleration_rate = 8.0f;
    deceleration_rate = 5.0f;
    moving_forward = false;
    moving_backward = false;
    moving_left = false;
    moving_right = false;
}

void Ship::Update(float delta_time) {
    // Calculate desired velocity based on input
    glm::vec3 desired_velocity(0.0f);

    // Get ship's forward and right directions
    glm::mat4 orientation_mat = glm::mat4_cast(orientation);
    glm::vec3 forward = glm::vec3(orientation_mat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    glm::vec3 right = glm::vec3(orientation_mat * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));

    if (moving_forward) desired_velocity += forward * max_speed;
    if (moving_backward) desired_velocity -= forward * max_speed * 0.5f;
    if (moving_left) desired_velocity -= right * max_speed * 0.7f;
    if (moving_right) desired_velocity += right * max_speed * 0.7f;

    // Smooth acceleration/deceleration
    if (glm::length(desired_velocity) > 0.001f) {
        // Accelerate towards desired velocity
        glm::vec3 velocity_diff = desired_velocity - velocity;
        float diff_length = glm::length(velocity_diff);
        if (diff_length > 0.001f) {
            acceleration = glm::normalize(velocity_diff) * acceleration_rate;
        }
    } else {
        // Decelerate to stop
        if (glm::length(velocity) > 0.001f) {
            acceleration = -glm::normalize(velocity) * deceleration_rate;
        } else {
            velocity = glm::vec3(0.0f);
            acceleration = glm::vec3(0.0f);
        }
    }

    // Update velocity
    velocity += acceleration * delta_time;

    // Clamp to max speed
    float speed = glm::length(velocity);
    if (speed > max_speed) {
        velocity = glm::normalize(velocity) * max_speed;
    }

    // Update position
    position += velocity * delta_time;

    // Call base update for children
    SceneNode::Update(delta_time);
}

glm::vec3 Ship::GetForward() {
    glm::mat4 orientation_mat = glm::mat4_cast(orientation);
    return glm::vec3(orientation_mat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
}
