#pragma once

#include "trianglemesh.h"
#include <glad/glad.h>

class Cube : public TriangleMesh
{
public:
    Cube(GLfloat x, GLfloat y, GLfloat z, bool inverse = false);
};
