#include "model.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

Model::Model(TriangleMesh* mesh) :
    m_mesh(mesh)
{}

Model::Model(TriangleMesh* mesh, const glm::vec3& origin, const glm::vec3& angles, const glm::vec3& albedo, float roughness, float metallic, const glm::vec3& emissive) :
    SceneNode(origin, angles),
    m_mesh(mesh),
    m_albedo(albedo),
    m_roughness(roughness),
    m_metallic(metallic),
    m_emissive(emissive)
{}

void Model::render()
{
    m_mesh->render();
}
