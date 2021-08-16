#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "glslprogram.h"

class SceneNode
{
protected:
    // Position
    glm::vec3 m_origin;

    // Euler angles here are pitch-yaw-roll.
    glm::vec3 m_angles;

    // Concatenation of the two transforms above.
    // Node space -> world space.
    glm::mat4 m_modelMatrix;

    //SceneNode* m_parent;
    //std::vector<SceneNode*> m_children;

public:
    SceneNode() {}
    SceneNode(glm::vec3 origin, glm::vec3 angles) :
        m_origin(origin),
        m_angles(angles)
    {}

    // CKG:
    // Matrix to convert the rotation
    // from classic pitch-yaw-roll CS (right-down-forward)
    // to the current CS (right, up, back)
    // for ease of use of Euler angles.
    static const glm::mat4 s_flipMatrix;

    void setOrigin(const glm::vec3& v) { m_origin = v; }
    const glm::vec3& getOrigin() const { return m_origin; }

    void setAngles(const glm::vec3& v) { m_angles = v; }
    const glm::vec3& getAngles() const { return m_angles; }

    virtual void updateMatrix();
    const glm::mat4& getModelMatrix() const { return m_modelMatrix; }
};
