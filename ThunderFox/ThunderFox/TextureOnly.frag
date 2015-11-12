#version 330 core

in vec2 UV;

out vec4 FragColor;

uniform sampler2D Tex_render;
uniform sampler2D Tex_visibility;

void main(){
	//float visibility = texture(Tex_visibility, UV).x;
	float visibility = 1.0;
	FragColor = visibility * texture(Tex_render, UV);
}