#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out VertexData{
	out vec3 Position_worldspace;
	out vec2 UV;
	out vec3 Normal_cameraspace;
	out vec3 EyeDirection_cameraspace;
	out vec3 LightDirection_cameraspace;
} VertexOut;

uniform mat4 M, V, P;
uniform vec3 LightPosition_worldspace;
uniform bool Test;

void main(){
	VertexOut.Position_worldspace = (M * vec4(vertexPosition_modelspace, 1)).xyz;
	//gl_Position = P * V * vec4(Position_worldspace, 1);

	vec3 vertexPosition_cameraspace = (V * vec4(VertexOut.Position_worldspace, 1)).xyz;
	VertexOut.EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;
	gl_Position = P * vec4(vertexPosition_cameraspace, 1);

	vec3 LightPosition_cameraspace = (V * vec4(LightPosition_worldspace, 1)).xyz;
	VertexOut.LightDirection_cameraspace = LightPosition_cameraspace + VertexOut.EyeDirection_cameraspace;

	VertexOut.Normal_cameraspace = (V * M * vec4(vertexNormal_modelspace, 0)).xyz;

	VertexOut.UV = vertexUV;
}