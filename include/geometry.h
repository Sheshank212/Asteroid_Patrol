#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "model.h"
#include <glm/glm.hpp>

// Helper function for HSV to RGB conversion
glm::vec3 HSVtoRGB(float h, float s, float v);

// Shape creation functions
Model* CreateCube(float size, glm::vec3 color);
Model* CreateSphere(float radius, int latitudes, int longitudes, glm::vec3 color);
Model* CreateCylinder(float radius, float height, int segments, glm::vec3 color);

#endif // GEOMETRY_H
