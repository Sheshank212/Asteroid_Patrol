#include "scene_node.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

SceneNode::SceneNode(std::string node_name)
    : name(node_name), position(0.0f), orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      scale(1.0f), parent(nullptr), model(nullptr), color(1.0f), visible(true) {}

SceneNode::~SceneNode() {
    for (auto child : children) {
        delete child;
    }
}

void SceneNode::AddChild(SceneNode* child) {
    children.push_back(child);
    child->parent = this;
}

// Get world transformation matrix by combining with parent transformations
glm::mat4 SceneNode::GetWorldTransform() {
    glm::mat4 local_transform = glm::mat4(1.0f);
    local_transform = glm::translate(local_transform, position);
    local_transform = local_transform * glm::mat4_cast(orientation);
    local_transform = glm::scale(local_transform, scale);

    if (parent) {
        return parent->GetWorldTransform() * local_transform;
    }
    return local_transform;
}

// Draw this node and all children recursively
void SceneNode::Draw(GLuint program) {
    if (visible && model) {
        glm::mat4 world_transform = GetWorldTransform();

        // Set world matrix
        GLint world_mat = glGetUniformLocation(program, "world_mat");
        glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(world_transform));

        // Set normal matrix
        glm::mat4 normal_matrix = glm::transpose(glm::inverse(world_transform));
        GLint normal_mat = glGetUniformLocation(program, "normal_mat");
        glUniformMatrix4fv(normal_mat, 1, GL_FALSE, glm::value_ptr(normal_matrix));

        // Bind and draw
        glBindBuffer(GL_ARRAY_BUFFER, model->vbo);

        GLint vertex_att = glGetAttribLocation(program, "vertex");
        glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), 0);
        glEnableVertexAttribArray(vertex_att);

        GLint normal_att = glGetAttribLocation(program, "normal");
        glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glEnableVertexAttribArray(normal_att);

        GLint color_att = glGetAttribLocation(program, "color");
        glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
        glEnableVertexAttribArray(color_att);

        if (model->use_elements) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
            glDrawElements(GL_TRIANGLES, model->size, GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, model->size);
        }
    }

    // Draw children
    for (auto child : children) {
        child->Draw(program);
    }
}

void SceneNode::Update(float delta_time) {
    for (auto child : children) {
        child->Update(delta_time);
    }
}
