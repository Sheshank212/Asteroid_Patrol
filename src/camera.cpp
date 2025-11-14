#include "camera.h"
#include <iostream>
#include <cmath>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() : SceneNode("Camera") {
    is_first_person = false;
    offset_third_person = glm::vec3(0.0f, 3.0f, 10.0f);
}

glm::mat4 Camera::GetViewMatrix() {
    glm::mat4 world_transform = GetWorldTransform();
    return glm::inverse(world_transform);
}

void Camera::ToggleView() {
    is_first_person = !is_first_person;
    std::cout << "Camera switched to " << (is_first_person ? "FIRST-PERSON" : "THIRD-PERSON") << " view" << std::endl;
}

void Camera::UpdateCameraPosition(SceneNode* ship) {
    if (is_first_person) {
        // First-person: camera at ship position
        position = glm::vec3(0.0f, 0.5f, 0.0f); // Slightly above ship center
        orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Look forward
    } else {
        // Third-person: camera behind and above ship
        position = offset_third_person;
        // Look at ship from behind
        glm::vec3 look_direction = glm::normalize(-offset_third_person);
        float pitch = asin(look_direction.y);
        glm::quat pitch_rot = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        orientation = pitch_rot;
    }
}
