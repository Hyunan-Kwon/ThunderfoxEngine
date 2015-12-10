#version 430

layout(binding = 0) uniform sampler2D tex_diffuse;

layout(location = 0) out vec3 position;
layout(location = 1) out vec4 albedo;
layout(location = 2) out vec2 normal;

in vec3 Position_worldspace;
in vec2 Texcoord;
in vec3 Normal_worldspace;

uniform float MaterialShininess;

void main(){
	position = Position_worldspace;
	albedo = vec4(texture(tex_diffuse, Texcoord).rgb, MaterialShininess);
	normal = normalize(Normal_worldspace).xy;
}