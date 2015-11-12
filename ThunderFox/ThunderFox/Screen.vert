#version 330 core

layout(location = 0) in vec2 vertexPosition_modelspace;

out vec2 UV;

void main(){
	gl_Position = vec4(vertexPosition_modelspace, 0.0, 1.0);
	UV = (vertexPosition_modelspace.xy + vec2(1.0, 1.0)) / 2.0;
}