#ifndef CAMERA_H
#define CAMERA_H

#include "scene_node.h"

// Camera class - Implements camera as a scene node
class Camera : public SceneNode {
public:
    bool is_first_person;
    glm::vec3 offset_third_person; // Offset for third-person view

    Camera();
    glm::mat4 GetViewMatrix();
    void ToggleView();
    void UpdateCameraPosition(SceneNode* ship);
};

#endif // CAMERA_H
