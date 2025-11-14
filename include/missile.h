#ifndef MISSILE_H
#define MISSILE_H

#include "scene_node.h"

// Missile class - Bonus weapon system
class Missile : public SceneNode {
public:
    float speed;
    float lifetime;
    float max_lifetime;
    bool active;

    Missile();
    void Fire(glm::vec3 start_pos, glm::quat start_orientation);
    void Update(float delta_time) override;
    glm::vec3 GetRayStart();
    glm::vec3 GetRayDirection();
};

#endif // MISSILE_H
