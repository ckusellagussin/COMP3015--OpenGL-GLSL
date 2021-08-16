#version 460

#define MAX_LIGHT_COUNT 3

uniform struct LightInfo 
{
	// Generic
	vec3 PositionWS;
	vec3 Color;
	
    float DistanceFalloffExp;

	// Converts vector from world space to this light's view space
	mat4 WorldToLightMatrix;
	mat4 ProjectionMatrix;

	sampler2DShadow ShadowTexture;

	// Attenuation factors
	// XYZ: 1 / radius (for omni)
	// XY: 1 / tan(fov), Z: 1 / distance (for spot)
	vec3 InverseScale;

	// Omni

	// Spot
    float ConeHotspot;
    float ConeHotspotInverse;
    float ConeFalloffExp;
} Light[MAX_LIGHT_COUNT];

uniform int LightCount;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ModelViewProjectionMatrix;
uniform vec3 ViewPositionWS;

// Model space position & normal
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal; 

out vec3 VertexPositionWS;
out vec3 NormalWS;
out vec3 ViewVectorWS;
out vec3 LightVectorWS[MAX_LIGHT_COUNT];
out vec3 VertexPositionLS[MAX_LIGHT_COUNT];
out vec4 ShadowTC[MAX_LIGHT_COUNT];

void main() 
{ 
	// World space position & normal
	VertexPositionWS = (ModelMatrix * vec4(VertexPosition, 1.0)).xyz;
	NormalWS = NormalMatrix * VertexNormal;
	ViewVectorWS = ViewPositionWS - VertexPositionWS;

	for (int i = 0; i < LightCount; i++)
	{
		// World space light vector
		LightVectorWS[i] = Light[i].PositionWS - VertexPositionWS;

		// Light space position
		VertexPositionLS[i] = (Light[i].WorldToLightMatrix * vec4(VertexPositionWS, 1.0)).xyz;
		
		// Shadow texture coordinates
		ShadowTC[i] = Light[i].ProjectionMatrix * vec4(VertexPositionLS[i], 1.0);
	}

	// Clip space position
	gl_Position = ModelViewProjectionMatrix * vec4(VertexPosition, 1.0);
}
