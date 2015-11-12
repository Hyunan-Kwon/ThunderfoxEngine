#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D renderedTexture;
uniform vec2 shift;

void main(){

	vec4 centerColor = texture(renderedTexture, UV);
	vec4 leftColor = texture(renderedTexture, UV + shift);
	vec4 rightColor = texture(renderedTexture, UV - shift);

	//if(centerColor.a > 0.0){
	//	color = centerColor;
	//}
	//else if(leftColor.a > 0.0){
	//	color = vec4(leftColor.rgb * 0.5, 1.0);
	//}
	//else if(rightColor.a > 0.0){
	//	color = vec4(rightColor.rgb * 0.5, 1.0);
	//}
	//else{
	//	discard;
	//}
	color = centerColor;
}