#include "cube.h"
#include <glad/glad.h>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <glm/gtc/constants.hpp>

//
// x, y, z are the half-widths on each axis
//
Cube::Cube(GLfloat x, GLfloat y, GLfloat z, bool inverse)
{
    // 6 faces. Each is separate, so vertices do not share their normals.

    // 24 vertices.

    // Points.
    std::vector<GLfloat> p{
        x, y, -z,
        -x, y, -z,
        -x, y, z,
        x, y, z,

        x, -y, z,
        -x, -y, z,
        -x, -y, -z,
        x, -y, -z,

        x, y, z,
        -x, y, z,
        -x, -y, z,
        x, -y, z,

        x, -y, -z,
        -x, -y, -z,
        -x, y, -z,
        x, y, -z,

        -x, y, z,
        -x, y, -z,
        -x, -y, -z,
        -x, -y, z,

        x, y, -z,
        x, y, z,
        x, -y, z,
        x, -y, -z
    };

    // Normals.
    std::vector<GLfloat> n{
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,

        0.f, -1.f, 0.f,
        0.f, -1.f, 0.f,
        0.f, -1.f, 0.f,
        0.f, -1.f, 0.f,

        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,

        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,

        -1.f, 0.f, 0.f,
        -1.f, 0.f, 0.f,
        -1.f, 0.f, 0.f,
        -1.f, 0.f, 0.f,

        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f
    };

    // Tex coords.
    std::vector<GLfloat> tex{
        1.f, 0.f,
        0.f, 0.f,
        0.f, 1.f,
        1.f, 1.f,

        1.f, 1.f,
        0.f, 1.f,
        0.f, 0.f,
        1.f, 0.f,

        1.f, 1.f,
        0.f, 1.f,
        0.f, 0.f,
        1.f, 0.f,

        1.f, 0.f,
        0.f, 0.f,
        0.f, 1.f,
        1.f, 1.f,

        1.f, 1.f,
        1.f, 0.f,
        0.f, 0.f,
        0.f, 1.f,

        1.f, 0.f,
        1.f, 1.f,
        0.f, 1.f,
        0.f, 0.f
    };

    // 36 elements.
    std::vector<GLuint> el;
    
    if (inverse)
    {
        // Invert normals.
        for_each(n.begin(), n.end(), [](float& f) { f = -f; });

        el = {
            2, 1, 0,
            0, 3, 2,

            6, 5, 4,
            4, 7, 6,

            10, 9, 8,
            8, 11, 10,

            14, 13, 12,
            12, 15, 14,

            18, 17, 16,
            16, 19, 18,

            22, 21, 20,
            20, 23, 22
        };
    }
    else
    {
        el = {
            0, 1, 2,
            2, 3, 0,

            4, 5, 6,
            6, 7, 4,

            8, 9, 10,
            10, 11, 8,

            12, 13, 14,
            14, 15, 12,

            16, 17, 18,
            18, 19, 16,

            20, 21, 22,
            22, 23, 20
        };
    }

    initBuffers(&el, &p, &n, &tex);
}

