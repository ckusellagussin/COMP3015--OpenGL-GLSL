//
// Can add support for directional light sources.
//

#include "scenenode.h"

class Light :
	public SceneNode
{
protected:
	glm::vec3 m_color;

	// Falloff power over distance from the origin
	float m_distanceFalloffExp;

	glm::mat4 m_projectionMatrix;

public:
	Light() {}

	Light(
		const glm::vec3& origin,
		const glm::vec3& angles,
		const glm::vec3& color,
		float distanceFalloffExp
	) :
		SceneNode(origin, angles),
		m_color(color),
		m_distanceFalloffExp(distanceFalloffExp)
	{}

	void setColor(const glm::vec3& x) { m_color = x; }
	const glm::vec3 &getColor() const { return m_color; }

	void setDistanceFalloffExp(float x) { m_distanceFalloffExp = x; }
	float getDistanceFalloffExp() const { return m_distanceFalloffExp; }

	const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
};

// Point light, shines in all directions
class Light_Omni :
	public Light
{
protected:
	// Radius beyond which intensity is zero
	glm::vec3 m_radius;

public:
	Light_Omni() {}

	Light_Omni(
		const glm::vec3& origin,
		const glm::vec3& angles,
		const glm::vec3& color,
		float distanceFalloffExp,
		const glm::vec3& radius
	) :
		Light(origin, angles, color, distanceFalloffExp),
		m_radius(radius)
	{}
		
	void setRadius(const glm::vec3& x) { m_radius = x; }
	const glm::vec3& getRadius() const { return m_radius; }

//	void updateMatrix();
};

// Point light, shining in one direction
class Light_Spot :
	public Light
{
protected:
	// Distance beyond which intensity is zero
	float m_distance;
	
	// Field of view
	glm::vec2 m_fov;

	// Relative size of inner, full-bright, circle (0 to 1)
	float m_hotspot;

	// Falloff power between hotspot edge & outer edge
	float m_coneFalloffExp;

public:
	Light_Spot() {}

	Light_Spot(
		const glm::vec3& origin,
		const glm::vec3& angles,
		const glm::vec3& color,
		float distanceFalloffExp,
		float distance,
		const glm::vec2& fov,
		float hotspot,
		float coneFalloffExp
	) :
		Light(origin, angles, color, distanceFalloffExp),
		m_distance(distance),
		m_fov(fov),
		m_hotspot(hotspot),
		m_coneFalloffExp(coneFalloffExp)
	{}

	void setDistance(float x) { m_distance = x; }
	float getDistance() const { return m_distance; }

	void setFov(const glm::vec3& x) { m_fov = x; }
	const glm::vec2& getFov() const { return m_fov; }

	void setHotspot(float x) { m_hotspot = x; }
	float getHotspot() const { return m_hotspot; }

	void setConeFalloffExp(float x) { m_coneFalloffExp = x; }
	float getConeFalloffExp() const { return m_coneFalloffExp; }

	virtual void updateMatrix();
};
