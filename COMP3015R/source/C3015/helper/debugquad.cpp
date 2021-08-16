#include "debugquad.h"
#include <glad/glad.h>

DebugQuad::DebugQuad()
{
    std::vector<GLfloat> n(12);

    //
    // Fullscreen quad. Matrix-scaled later.
    //

    std::vector<GLfloat> p
    {
        0.f, 0.f, 0.f,
        0.f, 1.f, 0.f,
        1.f, 1.f, 0.f,
        1.f, 0.f, 0.f
    };

    std::vector<GLfloat> tex
    {
        0.f, 0.f,
        0.f, 1.f,
        1.f, 1.f,
        1.f, 0.f
    };

    std::vector<GLuint> el{ 0, 1, 2, 2, 3, 0 };

    initBuffers(&el, &p, &n, &tex);
}
