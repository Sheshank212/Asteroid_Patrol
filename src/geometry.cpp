#include "geometry.h"
#include <GL/glew.h>
#include <glm/gtc/constants.hpp>
#include <cmath>

// Helper function for HSV to RGB conversion
glm::vec3 HSVtoRGB(float h, float s, float v) {
    float c = v * s;
    float x = c * (1.0f - abs(fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    glm::vec3 rgb;
    if (h >= 0 && h < 60) rgb = glm::vec3(c, x, 0);
    else if (h < 120) rgb = glm::vec3(x, c, 0);
    else if (h < 180) rgb = glm::vec3(0, c, x);
    else if (h < 240) rgb = glm::vec3(0, x, c);
    else if (h < 300) rgb = glm::vec3(x, 0, c);
    else rgb = glm::vec3(c, 0, x);
    return rgb + glm::vec3(m);
}

Model* CreateCube(float size, glm::vec3 color) {
    float h = size / 2.0f;
    const int vertex_num = 24; // 6 faces * 4 vertices
    const int face_num = 12;   // 6 faces * 2 triangles
    const int vertex_att = 9;  // pos(3) + normal(3) + color(3)
    const int face_att = 3;

    GLfloat vertex[vertex_num * vertex_att] = {
        // Front
        -h, -h,  h,  0, 0, 1,  color.r, color.g, color.b,
         h, -h,  h,  0, 0, 1,  color.r, color.g, color.b,
         h,  h,  h,  0, 0, 1,  color.r, color.g, color.b,
        -h,  h,  h,  0, 0, 1,  color.r, color.g, color.b,
        // Back
         h, -h, -h,  0, 0, -1,  color.r, color.g, color.b,
        -h, -h, -h,  0, 0, -1,  color.r, color.g, color.b,
        -h,  h, -h,  0, 0, -1,  color.r, color.g, color.b,
         h,  h, -h,  0, 0, -1,  color.r, color.g, color.b,
        // Left
        -h, -h, -h,  -1, 0, 0,  color.r, color.g, color.b,
        -h, -h,  h,  -1, 0, 0,  color.r, color.g, color.b,
        -h,  h,  h,  -1, 0, 0,  color.r, color.g, color.b,
        -h,  h, -h,  -1, 0, 0,  color.r, color.g, color.b,
        // Right
         h, -h,  h,  1, 0, 0,  color.r, color.g, color.b,
         h, -h, -h,  1, 0, 0,  color.r, color.g, color.b,
         h,  h, -h,  1, 0, 0,  color.r, color.g, color.b,
         h,  h,  h,  1, 0, 0,  color.r, color.g, color.b,
        // Top
        -h,  h,  h,  0, 1, 0,  color.r, color.g, color.b,
         h,  h,  h,  0, 1, 0,  color.r, color.g, color.b,
         h,  h, -h,  0, 1, 0,  color.r, color.g, color.b,
        -h,  h, -h,  0, 1, 0,  color.r, color.g, color.b,
        // Bottom
        -h, -h, -h,  0, -1, 0,  color.r, color.g, color.b,
         h, -h, -h,  0, -1, 0,  color.r, color.g, color.b,
         h, -h,  h,  0, -1, 0,  color.r, color.g, color.b,
        -h, -h,  h,  0, -1, 0,  color.r, color.g, color.b,
    };

    GLuint face[face_num * face_att] = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        8,9,10, 10,11,8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20
    };

    Model* model = new Model;
    model->size = face_num * face_att;
    model->use_elements = true;

    glGenBuffers(1, &model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &model->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face), face, GL_STATIC_DRAW);

    return model;
}

Model* CreateSphere(float radius, int latitudes, int longitudes, glm::vec3 color) {
    const int vertex_num = (latitudes + 1) * (longitudes + 1);
    const int face_num = latitudes * longitudes * 2;
    const int vertex_att = 9;
    const int face_att = 3;

    GLfloat* vertex = new GLfloat[vertex_num * vertex_att];
    GLuint* face = new GLuint[face_num * face_att];

    int v_idx = 0, f_idx = 0;

    for (int lat = 0; lat <= latitudes; lat++) {
        float theta = glm::pi<float>() * lat / latitudes;
        float sin_theta = sin(theta);
        float cos_theta = cos(theta);

        for (int lon = 0; lon <= longitudes; lon++) {
            float phi = 2.0f * glm::pi<float>() * lon / longitudes;
            float sin_phi = sin(phi);
            float cos_phi = cos(phi);

            float x = radius * sin_theta * cos_phi;
            float y = radius * cos_theta;
            float z = radius * sin_theta * sin_phi;

            vertex[v_idx++] = x;
            vertex[v_idx++] = y;
            vertex[v_idx++] = z;
            vertex[v_idx++] = x / radius;
            vertex[v_idx++] = y / radius;
            vertex[v_idx++] = z / radius;
            vertex[v_idx++] = color.r;
            vertex[v_idx++] = color.g;
            vertex[v_idx++] = color.b;
        }
    }

    for (int lat = 0; lat < latitudes; lat++) {
        for (int lon = 0; lon < longitudes; lon++) {
            int current = lat * (longitudes + 1) + lon;
            int next = current + longitudes + 1;

            face[f_idx++] = current;
            face[f_idx++] = next;
            face[f_idx++] = current + 1;

            face[f_idx++] = current + 1;
            face[f_idx++] = next;
            face[f_idx++] = next + 1;
        }
    }

    Model* model = new Model;
    model->size = face_num * face_att;
    model->use_elements = true;

    glGenBuffers(1, &model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &model->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

    delete[] vertex;
    delete[] face;

    return model;
}

Model* CreateCylinder(float radius, float height, int segments, glm::vec3 color) {
    const int vertex_num = segments * 2 + 2; // sides + top/bottom centers
    const int face_num = segments * 4; // sides (2 per segment) + caps (1 per segment each)
    const int vertex_att = 9;
    const int face_att = 3;

    GLfloat* vertex = new GLfloat[vertex_num * vertex_att];
    GLuint* face = new GLuint[face_num * face_att];

    int v_idx = 0, f_idx = 0;
    float half_height = height / 2.0f;

    // Side vertices
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * glm::pi<float>() * i / segments;
        float cos_a = cos(angle);
        float sin_a = sin(angle);

        // Bottom
        vertex[v_idx++] = radius * cos_a;
        vertex[v_idx++] = -half_height;
        vertex[v_idx++] = radius * sin_a;
        vertex[v_idx++] = cos_a;
        vertex[v_idx++] = 0;
        vertex[v_idx++] = sin_a;
        vertex[v_idx++] = color.r;
        vertex[v_idx++] = color.g;
        vertex[v_idx++] = color.b;

        // Top
        vertex[v_idx++] = radius * cos_a;
        vertex[v_idx++] = half_height;
        vertex[v_idx++] = radius * sin_a;
        vertex[v_idx++] = cos_a;
        vertex[v_idx++] = 0;
        vertex[v_idx++] = sin_a;
        vertex[v_idx++] = color.r;
        vertex[v_idx++] = color.g;
        vertex[v_idx++] = color.b;
    }

    // Center vertices for caps
    int top_center = segments * 2;
    int bottom_center = segments * 2 + 1;

    vertex[v_idx++] = 0; vertex[v_idx++] = half_height; vertex[v_idx++] = 0;
    vertex[v_idx++] = 0; vertex[v_idx++] = 1; vertex[v_idx++] = 0;
    vertex[v_idx++] = color.r; vertex[v_idx++] = color.g; vertex[v_idx++] = color.b;

    vertex[v_idx++] = 0; vertex[v_idx++] = -half_height; vertex[v_idx++] = 0;
    vertex[v_idx++] = 0; vertex[v_idx++] = -1; vertex[v_idx++] = 0;
    vertex[v_idx++] = color.r; vertex[v_idx++] = color.g; vertex[v_idx++] = color.b;

    // Side faces
    for (int i = 0; i < segments; i++) {
        int curr_b = i * 2;
        int curr_t = i * 2 + 1;
        int next_b = ((i + 1) % segments) * 2;
        int next_t = ((i + 1) % segments) * 2 + 1;

        face[f_idx++] = curr_b; face[f_idx++] = next_b; face[f_idx++] = curr_t;
        face[f_idx++] = curr_t; face[f_idx++] = next_b; face[f_idx++] = next_t;

        // Top cap
        face[f_idx++] = top_center; face[f_idx++] = curr_t; face[f_idx++] = next_t;

        // Bottom cap
        face[f_idx++] = bottom_center; face[f_idx++] = next_b; face[f_idx++] = curr_b;
    }

    Model* model = new Model;
    model->size = face_num * face_att;
    model->use_elements = true;

    glGenBuffers(1, &model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &model->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

    delete[] vertex;
    delete[] face;

    return model;
}
