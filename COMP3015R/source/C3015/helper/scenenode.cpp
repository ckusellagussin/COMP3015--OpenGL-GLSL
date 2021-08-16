#include "scenenode.h"
#include <string>

// CKG:
// Matrix to convert the rotation
// from classic pitch-yaw-roll CS (right, down, forward)
// to the current coordinate space (right, up, back).
const glm::mat4 SceneNode::s_flipMatrix{
    1.f, 0.f, 0.f, 0.f,
    0.f, -1.f, 0.f, 0.f,
    0.f, 0.f, -1.f, 0.f,
    0.f, 0.f, 0.f, 1.f
};

void SceneNode::updateMatrix()
{
    m_modelMatrix = glm::translate(glm::mat4(1.f), m_origin);

    m_modelMatrix *= s_flipMatrix * glm::yawPitchRoll(
        glm::radians(m_angles[1]),
        glm::radians(m_angles[0]),
        glm::radians(m_angles[2])
    ) * s_flipMatrix;
}
