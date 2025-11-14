#ifndef ASTEROID_H
#define ASTEROID_H

#include "scene_node.h"

// Asteroid class with collision detection
class Asteroid : public SceneNode {
public:
    float radius;
    bool hit;

    Asteroid();
    bool CheckRayIntersection(glm::vec3 ray_origin, glm::vec3 ray_direction);
    bool CheckMissileIntersection(glm::vec3 missile_pos, float missile_radius);
};

#endif // ASTEROID_H
