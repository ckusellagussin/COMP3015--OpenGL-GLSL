#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include <glad/glad.h>
#include "helper/glslprogram.h"
#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/cube.h"
#include "helper/debugquad.h"
#include "helper/model.h"
#include "helper/light.h"
#include <glm/glm.hpp>

class SceneBasic_Uniform : public Scene
{
private:
    static const float s_shadowSize[2];
    
    GLSLProgram m_basicProgram;
    GLuint m_pass1;
    GLuint m_pass2;

    GLSLProgram m_shadowProgram;
    GLSLProgram m_debugProgram;

    // Use locally generated data so we don't have to introduce allocator.
    Torus m_torusMesh;
    Teapot m_teapotMesh;
    Cube m_cubeMesh;
    DebugQuad m_debugQuadMesh;

    Model m_torus;
    Model m_teapot;
    Model m_cube;

    std::vector<Light_Spot> m_spotLights;
    std::vector<GLuint> m_shadowTextures;
    std::vector<GLuint> m_shadowFBO;

    // CKG:
    // List of references to the above.
    // Make scene tree later.
    std::vector<Model*> m_models;
    std::vector<Light*> m_lights;

    void compile();

    //
    // CKG: Camera.
    //

    // Generic setup.
    //glm::vec3 m_viewOrigin;
    //glm::vec3 m_viewAngles;

    // Orbiting setup.

    // Distance from the center.
    float m_viewDistance;
    float m_viewDistanceSpeed;

    // TODO: Make camera a scene node too.
    
    // Elevation & azimuth angles.
    glm::vec2 m_viewAngles;

    // Turning speed in while key is pressed (degrees/sec).
    glm::vec2 m_viewAngleSpeed;

public:
    SceneBasic_Uniform();
    ~SceneBasic_Uniform();

    void initScene();
    void update(float time);
    void render();
    void resize(int, int);
    void keyCallback(int key, int scancode, int action, int mods);
};

#endif // SCENEBASIC_UNIFORM_H
