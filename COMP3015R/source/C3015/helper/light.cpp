#include "light.h"
#include <glm/glm.hpp>

void Light_Spot::updateMatrix()
{
	SceneNode::updateMatrix();

	m_projectionMatrix = glm::perspective(glm::radians(m_fov[1]), m_fov[0] / m_fov[1], 0.3f, 100.f);
}
