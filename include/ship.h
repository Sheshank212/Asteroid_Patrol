#ifndef SHIP_H
#define SHIP_H

#include "scene_node.h"

// Ship class with physics-based movement
class Ship : public SceneNode {
public:
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float max_speed;
    float acceleration_rate;
    float deceleration_rate;
    bool moving_forward;
    bool moving_backward;
    bool moving_left;
    bool moving_right;

    Ship();
    void Update(float delta_time) override;
    glm::vec3 GetForward();
};

#endif // SHIP_H
