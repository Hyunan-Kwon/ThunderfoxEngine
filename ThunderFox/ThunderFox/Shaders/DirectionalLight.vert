#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out VertexData{
    vec3 Normal_cameraspace;
    vec2 UV;
    vec3 EyeDirection_cameraspace;
    vec4 ShadowCoord;
    vec4 Position_worldspace;
    vec4 Position_cameraspace;
} VertexOut;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 DepthMVP;

void main(){
	VertexOut.Position_worldspace = M * vec4(vertexPosition_modelspace, 1);
	vec4 vertexPosition_cameraspace = V * VertexOut.Position_worldspace;
	VertexOut.Position_cameraspace = vertexPosition_cameraspace;
	gl_Position = P * vertexPosition_cameraspace;

	VertexOut.UV = vertexUV;
	VertexOut.Normal_cameraspace = (transpose(inverse(V * M)) * vec4(vertexNormal_modelspace, 0)).xyz;
	VertexOut.EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace.xyz;

	VertexOut.ShadowCoord = DepthMVP * vec4(vertexPosition_modelspace, 1);
}