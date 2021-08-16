#version 460

uniform mat4 ModelViewProjectionMatrix;

layout (location = 0) in vec3 VertexPosition;
layout (location = 2) in vec2 VertexTC;

out vec2 DebugTC;

void main() 
{ 
	DebugTC = VertexTC;

	gl_Position = ModelViewProjectionMatrix * vec4(VertexPosition, 1.0);
}
