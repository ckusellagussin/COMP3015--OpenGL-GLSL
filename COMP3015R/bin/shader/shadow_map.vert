#version 460

uniform mat4 ModelViewProjectionMatrix;

// Model space position
layout (location = 0) in vec3 VertexPosition;

void main() 
{ 
	// Clip space position
	gl_Position = ModelViewProjectionMatrix * vec4(VertexPosition, 1.0);
}
