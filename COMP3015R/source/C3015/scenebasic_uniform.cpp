#include "scenebasic_uniform.h"
#include "helper/texture.h"
#include <GLFW/glfw3.h>

#include <iostream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;



const float SceneBasic_Uniform::s_shadowSize[2]{ 2048, 2048 };

SceneBasic_Uniform::SceneBasic_Uniform() :
    
    // Meshes
   
    //constructor for torus
    m_torusMesh(5.5f, 0.5f, 50, 50),
    
    //constructor for teapot
    m_teapotMesh(13, glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f))),
    
    m_cubeMesh(9.f, 9.f, 9.f, true),
 
    // Models   

    m_torus(
        &m_torusMesh,
        vec3(0.f),
        vec3(-35.f, 15.f, 0.f),
        vec3(0.25f, 1.f, 0.5f),
        0.5f,
        0.f,
        vec3(0.f)
    ),
    m_teapot(
        &m_teapotMesh,
        vec3(0.f, -1.f, 0.f),
        vec3(-90.f, 0.f, 0.f),
        vec3(1.f, 0.25f, 0.5f),
        0.1f,
        1.f,
        vec3(0.f)
    ),
    m_cube(
        &m_cubeMesh,
        vec3(0.f),
        vec3(0.f),
        vec3(1.f, 1.f, 1.f),
        0.8f,
        0.f,
        vec3(0.f)
    ),
    
    // Lights
    
    m_spotLights{
        Light_Spot(
            glm::vec3(0.f, 8.f, 0.f),
            glm::vec3(-75.f, 0.f, 0.f),
            glm::vec3(0.9f, 1.f, 0.9f),
            1.f,
            25.f,
            glm::vec2(90.f, 135.f),
            0.5f,
            1.f
        ),
        Light_Spot(
            glm::vec3(0.f, -8.f, 0.f),
            glm::vec3(90.f, 0.f, 0.f),
            glm::vec3(1.f, 0.4f, 0.3f),
            1.f,
            25.f,
            glm::vec2(170.f, 120.f),
            0.8f,
            2.f
        ),
        Light_Spot(
            glm::vec3(8.5f, 0.f, 8.5f),
            glm::vec3(0.f, -45.f, 0.f),
            glm::vec3(0.4f, 0.7f, 1.f),
            1.f,
            30.f,
            glm::vec2(105.f, 60.f),
            0.9f,
            3.f
        )
    }
{
    // Add the objects to the list
    m_models.push_back(&m_torus);
    m_models.push_back(&m_teapot);
    m_models.push_back(&m_cube);

    for (auto l = m_spotLights.begin(); l != m_spotLights.end(); l++)
        m_lights.push_back(&(*l));
}

SceneBasic_Uniform::~SceneBasic_Uniform()
{
    glDeleteTextures(GLuint(m_shadowTextures.size()), m_shadowTextures.data());
    glDeleteFramebuffers(GLuint(m_shadowFBO.size()), m_shadowFBO.data());
}

void SceneBasic_Uniform::initScene()
{
    // Initialize the timer.
    m_oldTime = m_deltaTime = 0.f;
    
    compile();

    // General settings.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Orbit
    m_viewDistance = 7.5f;
    m_viewAngles = vec2(0.f, 0.f);

    // Initialize shadow maps
    
    m_shadowTextures.resize(m_lights.size());
    m_shadowFBO.resize(m_lights.size());

    for (int i = 0; i < m_lights.size(); i++)
    {
        float border[4] = { 1.f, 0.f, 0.f, 0.f };

        // Texture
        glGenTextures(1, &m_shadowTextures[i]);
        glBindTexture(GL_TEXTURE_2D, m_shadowTextures[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, s_shadowSize[0], s_shadowSize[1]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

        #ifndef SHADOW_DEBUG
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
        #endif

        // Frame buffer
        glGenFramebuffers(1, &m_shadowFBO[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowTextures[i], 0);

        glReadBuffer(GL_NONE);
        glDrawBuffer(GL_NONE);

        GLenum r = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        printf("Shadow map framebuffer %i is %scomplete.\n", i, r == GL_FRAMEBUFFER_COMPLETE ? "" : "not ");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::compile()
{
	try {
		m_basicProgram.compileShader("shader/basic_uniform.vert");
		m_basicProgram.compileShader("shader/basic_uniform.frag");
		m_basicProgram.link();
     
    } catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}

    try {
        m_shadowProgram.compileShader("shader/shadow_map.vert");
        m_shadowProgram.compileShader("shader/shadow_map.frag");
        m_shadowProgram.link();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    try {
        m_debugProgram.compileShader("shader/debug.vert");
        m_debugProgram.compileShader("shader/debug.frag");
        m_debugProgram.link();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float time)
{
    // Update the timer.
    m_deltaTime = time - m_oldTime;
    m_oldTime = time;
   
    
    // Animate the scene 
    
    if (m_animate)
    {
        // Models
        m_torus.setAngles(glm::mod(m_torus.getAngles() + vec3(20.f, 40.f, 60.f) * m_deltaTime, 360.f));

        m_teapot.setAngles(glm::mod(m_teapot.getAngles() + vec3(0, -30.f, 0) * m_deltaTime, 360.f));

        // Lights
        m_spotLights[0].setAngles(glm::mod(m_spotLights[0].getAngles() + vec3(0.f, 45.f, 0.f) * m_deltaTime, 360.f));
        m_spotLights[1].setAngles(glm::mod(m_spotLights[1].getAngles() + vec3(0.f, 90.f, 0.f) * m_deltaTime, 360.f));
        m_spotLights[2].setAngles(glm::mod(m_spotLights[2].getAngles() + vec3(0.f, 0.f, 180.f) * m_deltaTime, 360.f));
    }

    // Camera.
    m_viewAngles[0] = glm::clamp(m_viewAngles[0] + m_viewAngleSpeed[0] * m_deltaTime, -89.99f, 89.99f);
    m_viewAngles[1] = glm::mod(m_viewAngles[1] + m_viewAngleSpeed[1] * m_deltaTime, 360.f);

    m_viewDistance = glm::clamp(m_viewDistance + m_viewDistanceSpeed * m_deltaTime, 4.f, 17.f);

    // Update model matrix for all nodes.
    for (auto m = m_models.begin(); m != m_models.end(); m++)
        (*m)->updateMatrix();

    for (auto l = m_lights.begin(); l != m_lights.end(); l++)
        (*l)->updateMatrix();
}

void SceneBasic_Uniform::render()
{
    glm::mat4 viewMatrix;
    int i;

   
    // Render the shadow maps.

    m_shadowProgram.use();
    //glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, m_pass1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.1f, 0.4f);

    i = 0;
    
    for (auto l = m_spotLights.begin(); l != m_spotLights.end(); l++, i++)
    {
        // Setup
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO[i]);
        glViewport(0, 0, s_shadowSize[0], s_shadowSize[1]);

        glClearDepth(1.f);
        glClear(GL_DEPTH_BUFFER_BIT);
    
        viewMatrix = glm::inverse(l->getModelMatrix());

        
        // Draw all objects      

        for (auto m = m_models.begin(); m != m_models.end(); m++)
        {
            m_shadowProgram.setUniform("ModelViewProjectionMatrix", l->getProjectionMatrix() * viewMatrix * (*m)->getModelMatrix());

            (*m)->render();
        }
    }

    //glFinish();
    glFlush();

    
    // Render the shaded scene
   
    m_basicProgram.use();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    glDisable(GL_POLYGON_OFFSET_FILL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);

    glClearColor(0.3f, 0.2f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    // Orbit the camera around the center of the scene.  

    // Compute eye position using two angles.
    float s0 = sinf(glm::radians(m_viewAngles[0]));
    float c0 = cosf(glm::radians(m_viewAngles[0]));
    float s1 = sinf(glm::radians(m_viewAngles[1]));
    float c1 = cosf(glm::radians(m_viewAngles[1]));

    vec3 viewOrigin = vec3(-s1 * c0, -s0, c1 * c0) * m_viewDistance;
    
#if 1
    viewMatrix = glm::lookAt(
        viewOrigin,
        vec3(0.f),
        vec3(0.f, 1.f, 0.f)
    );
#else
    
    viewMatrix = glm::translate(glm::mat4(1.f), viewOrigin);
    viewMatrix *= SceneNode::s_flipMatrix * glm::yawPitchRoll(
        glm::radians(m_viewAngles[1]),
        glm::radians(m_viewAngles[0]),
        glm::radians(0.f)
    ) * SceneNode::s_flipMatrix;
    viewMatrix = glm::inverse(viewMatrix);
#endif

    m_basicProgram.setUniform("ViewPositionWS", viewOrigin);

    
    // Send light information
   
    m_basicProgram.setUniform("LightCount", int(m_spotLights.size()));
    
    i = 0;

#if 1
    const mat4 shadowBiasMatrix(
        0.5f, 0.f, 0.f, 0.f,
        0.f, 0.5f, 0.f, 0.f,
        0.f, 0.f, 0.5f, 0.f,
        0.5f, 0.5f, 0.5f, 1.f
    );
#else
    const mat4 shadowBiasMatrix(
        0.5f, 0.f, 0.f, 0.5f,
        0.f, 0.5f, 0.f, 0.5f,
        0.f, 0.f, 0.5f, 0.5f,
        0.f, 0.f, 0.f, 1.f
    );
#endif

    for (auto l = m_spotLights.begin(); l != m_spotLights.end(); l++, i++)
    {
        std::string uniform("Light[" + std::to_string(i) + "].");
        
        m_basicProgram.setUniform(uniform + "Color", l->getColor());     //setting the Ld uniform
        m_basicProgram.setUniform(uniform + "PositionWS", l->getOrigin()); //setting Light Position

        m_basicProgram.setUniform(uniform + "DistanceFalloffExp", l->getDistanceFalloffExp());

        // Shadow map
        m_basicProgram.setUniform(uniform + "WorldToLightMatrix", glm::inverse(l->getModelMatrix()));
        m_basicProgram.setUniform(uniform + "ProjectionMatrix", shadowBiasMatrix * l->getProjectionMatrix());

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_shadowTextures[i]);

        m_basicProgram.setUniform(uniform + "ShadowTexture", i);

        // Multipliers to go into unit space for attenuation
        m_basicProgram.setUniform(uniform + "InverseScale",
            glm::vec3(
                1.f / tanf(0.5f * glm::radians(l->getFov()[0])),
                1.f / tanf(0.5f * glm::radians(l->getFov()[1])),
                1.f / l->getDistance()
            )
        );

        m_basicProgram.setUniform(uniform + "ConeHotspot", l->getHotspot());
        m_basicProgram.setUniform(uniform + "ConeHotspotInverse", 1.f / (1.f - l->getHotspot()));
        m_basicProgram.setUniform(uniform + "ConeFalloffExp", l->getConeFalloffExp());
    }

    
    // Draw all objects    

    for (auto m = m_models.begin(); m != m_models.end(); m++)
    {
        mat4 modelMatrix = (*m)->getModelMatrix();

        m_basicProgram.setUniform("ModelMatrix", modelMatrix);
        m_basicProgram.setUniform("NormalMatrix", glm::mat3(vec3(modelMatrix[0]), vec3(modelMatrix[1]), vec3(modelMatrix[2])));
        m_basicProgram.setUniform("ModelViewProjectionMatrix", m_projectionMatrix * viewMatrix * modelMatrix);

        m_basicProgram.setUniform("Material.Albedo", (*m)->getAlbedo());
        m_basicProgram.setUniform("Material.Roughness", (*m)->getRoughness());
        m_basicProgram.setUniform("Material.Metallic", (*m)->getMetallic());
        m_basicProgram.setUniform("Material.Emissive", (*m)->getEmissive());

        (*m)->render();
    }

#ifdef SHADOW_DEBUG
    
    // Debug render of a chosen light's shadow map.   

    m_debugProgram.use();

    glViewport(0, 0, 512, 512);
    mat4 orthoMatrix = glm::ortho(0.f, 512.f, 0.f, 512.f);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_shadowTextures[0]);

    m_debugProgram.setUniform("DebugTexture", 0);
    m_debugProgram.setUniform("ModelViewProjectionMatrix", glm::scale(orthoMatrix, vec3(512.f, 512.f, 0.f)));

    m_debugQuadMesh.render();
#endif
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);

    setDimensions(w, h);

    m_projectionMatrix = glm::perspectiveFov(glm::radians(90.f), float(w), float(h), 0.3f, 100.f);
}


// Handle keyboard input.

void SceneBasic_Uniform::keyCallback(int key, int scancode, int action, int mods)
{
    const float ZOOM_SPEED(10.f);
    const float ORBIT_SPEED[2]{ 135.f, 180.f };

    switch (key)
    {
    case GLFW_KEY_SPACE:
        if (action == GLFW_PRESS)
            m_animate = !m_animate;
        break;
    case GLFW_KEY_DOWN:
    case GLFW_KEY_KP_2:       
        if (action == GLFW_PRESS)
            m_viewAngleSpeed[0] = ORBIT_SPEED[0];
        else if (action == GLFW_RELEASE)
            m_viewAngleSpeed[0] = 0.f;
        break;
    case GLFW_KEY_UP:
    case GLFW_KEY_KP_8:
        if (action == GLFW_PRESS)
            m_viewAngleSpeed[0] = -ORBIT_SPEED[0];
        else if (action == GLFW_RELEASE)
            m_viewAngleSpeed[0] = 0.f;
        break;
    case GLFW_KEY_LEFT:
    case GLFW_KEY_KP_4:
        if (action == GLFW_PRESS)
            m_viewAngleSpeed[1] = ORBIT_SPEED[1];
        else if (action == GLFW_RELEASE)
            m_viewAngleSpeed[1] = 0.f;
        break;
    case GLFW_KEY_RIGHT:
    case GLFW_KEY_KP_6:
        if (action == GLFW_PRESS)
            m_viewAngleSpeed[1] = -ORBIT_SPEED[1];
        else if (action == GLFW_RELEASE)
            m_viewAngleSpeed[1] = 0.f;
        break;
    case GLFW_KEY_KP_SUBTRACT:
        if (action == GLFW_PRESS)
            m_viewDistanceSpeed = ZOOM_SPEED;
        else if (action == GLFW_RELEASE)
            m_viewDistanceSpeed = 0.f;
        break;
    case GLFW_KEY_KP_ADD:
        if (action == GLFW_PRESS)
            m_viewDistanceSpeed = -ZOOM_SPEED;
        else if (action == GLFW_RELEASE)
            m_viewDistanceSpeed = 0.f;
        break;
    }
}
