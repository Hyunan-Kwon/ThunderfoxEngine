#version 330 core

in vec2 UV;

out vec4 FragColor;

uniform sampler2D renderedTexture;

void main(){
	FragColor = texture2D(renderedTexture, UV);
}