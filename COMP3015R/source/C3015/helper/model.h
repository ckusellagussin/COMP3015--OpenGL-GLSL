#pragma once

#include "scenenode.h"
#include "trianglemesh.h"

class Model :
    public SceneNode
{
protected:
    // Single surface (mesh reference + material).
    // Can add support for multiple later.
    TriangleMesh* m_mesh;

    // Material parameters
    glm::vec3 m_albedo;
    float m_roughness;
    float m_metallic;
    glm::vec3 m_emissive;

public:
    Model(TriangleMesh* mesh);
    Model(TriangleMesh* mesh, const glm::vec3& origin, const glm::vec3& angles, const glm::vec3 &albedo, float roughness, float metallic, const glm::vec3& emissive);

    void setAlbedo(const glm::vec3& v) { m_albedo = v; }
    const glm::vec3& getAlbedo() const { return m_albedo; }

    void setRoughness(const float v) { m_roughness = v; }
    float getRoughness() const { return m_roughness; }

    void setMetallic(const float v) { m_metallic = v; }
    float getMetallic() const { return m_metallic; }

    void setEmissive(const glm::vec3& v) { m_emissive = v; }
    const glm::vec3& getEmissive() const { return m_emissive; }

    void render();
};
