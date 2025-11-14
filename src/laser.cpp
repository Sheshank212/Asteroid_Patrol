#include "laser.h"
#include <glm/gtc/matrix_transform.hpp>

Laser::Laser() : SceneNode("Laser") {
    speed = 50.0f;
    lifetime = 0.0f;
    max_lifetime = 3.0f;
    active = false;
    scale = glm::vec3(0.2f, 0.2f, 5.0f); // Long thin laser beam
}

void Laser::Fire(glm::vec3 start_pos, glm::quat start_orientation) {
    position = start_pos;
    orientation = start_orientation;
    active = true;
    lifetime = 0.0f;
    visible = true;
}

void Laser::Update(float delta_time) {
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

    SceneNode::Update(delta_time);
}

glm::vec3 Laser::GetRayStart() {
    return position;
}

glm::vec3 Laser::GetRayDirection() {
    glm::mat4 orientation_mat = glm::mat4_cast(orientation);
    return glm::normalize(glm::vec3(orientation_mat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
}
