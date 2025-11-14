#include "missile.h"
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

Missile::Missile() : SceneNode("Missile") {
    speed = 30.0f;
    lifetime = 0.0f;
    max_lifetime = 5.0f;
    active = false;
    scale = glm::vec3(0.3f, 0.3f, 1.0f);
}

void Missile::Fire(glm::vec3 start_pos, glm::quat start_orientation) {
    position = start_pos;
    orientation = start_orientation;
    active = true;
    lifetime = 0.0f;
    visible = true;
}

void Missile::Update(float delta_time) {
    if (!active) return;

    lifetime += delta_time;
    if (lifetime >= max_lifetime) {
        active = false;
        visible = false;
        return;
    }

    // Move forward
    glm::mat4 orientation_mat = glm::mat4_cast(orientation);
    glm::vec3 forward = glm::vec3(orientation_mat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    position += forward * speed * delta_time;

    // Add slight rotation for visual effect
    glm::quat rotation = glm::angleAxis(delta_time * 5.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    orientation = orientation * rotation;

    SceneNode::Update(delta_time);
}
glm::vec3 Missile::GetRayStart() {
    return position;
}

glm::vec3 Missile::GetRayDirection() {
    glm::mat4 orientation_mat = glm::mat4_cast(orientation);
    return glm::normalize(glm::vec3(orientation_mat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
}
