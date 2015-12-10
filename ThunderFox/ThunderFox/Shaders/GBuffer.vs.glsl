#version 330 core

layout(location = 0) in vec3 position_modelspace;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal_modelspace;

uniform mat4 M;
uniform mat4 PV;
uniform mat4 M_normal;

out vec3 Position_worldspace;
out vec2 Texcoord;
out vec3 Normal_worldspace;

void main(){
	Position_worldspace = (M * vec4(position_modelspace, 1.0)).xyz;
	Texcoord = texcoord;
	Normal_worldspace = (M_normal * vec4(normal_modelspace, 0.0)).xyz;
	gl_Position = PV * vec4(Position_worldspace, 1.0);
}