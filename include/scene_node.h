#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "model.h"

// SceneNode class - Base class for hierarchical scene graph
class SceneNode {
public:
    std::string name;
    glm::vec3 position;
    glm::quat orientation;
    glm::vec3 scale;
    SceneNode* parent;
    std::vector<SceneNode*> children;
    Model* model;
    glm::vec3 color;
    bool visible;

    SceneNode(std::string node_name);
    virtual ~SceneNode();

    void AddChild(SceneNode* child);
    glm::mat4 GetWorldTransform();
    virtual void Draw(GLuint program);
    virtual void Update(float delta_time);
};

#endif // SCENE_NODE_H
