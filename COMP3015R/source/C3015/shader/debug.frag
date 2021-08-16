#version 460

uniform sampler2D DebugTexture;

in vec2 DebugTC;

layout (location = 0) out vec4 FragColor;

void main()
{
	//FragColor.xyz = vec3(DebugTC.x, DebugTC.y, 0.0);
	//FragColor.xyz = texture(DebugTexture, vec3(DebugTC, DebugTC.x)).xxx;
	FragColor.xyz = texture(DebugTexture, DebugTC).xxx;
}
