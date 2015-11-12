#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 UV;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec4 ShadowCoord;
out vec4 Position_worldspace;
out vec4 Position_cameraspace;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 DepthMVP;

void main(){
	Position_worldspace = M * vec4(vertexPosition_modelspace, 1);
	vec4 vertexPosition_cameraspace = V * Position_worldspace;
	Position_cameraspace = vertexPosition_cameraspace;
	gl_Position = P * vertexPosition_cameraspace;

	UV = vertexUV;
	Normal_cameraspace = (transpose(inverse(V * M)) * vec4(vertexNormal_modelspace, 0)).xyz;
	EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace.xyz;

	ShadowCoord = DepthMVP * vec4(vertexPosition_modelspace, 1);
}