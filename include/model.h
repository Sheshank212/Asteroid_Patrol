#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>

// Store information of one model for rendering
typedef struct model {
    GLuint vbo;         // Vertex buffer object
    GLuint ebo;         // Element buffer object
    GLuint size;        // Number of vertices/elements
    bool use_elements;  // Whether to use element buffer
} Model;

#endif // MODEL_H
