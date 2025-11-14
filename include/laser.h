#ifndef LASER_H
#define LASER_H

#include "scene_node.h"

// Laser class - Modular weapon design
class Laser : public SceneNode {
public:
    float speed;
    float lifetime;
    float max_lifetime;
    bool active;

    Laser();
    void Fire(glm::vec3 start_pos, glm::quat start_orientation);
    void Update(float delta_time) override;
    glm::vec3 GetRayStart();
    glm::vec3 GetRayDirection();
};

#endif // LASER_H
