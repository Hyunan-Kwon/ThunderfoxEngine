#version 430

layout(binding = 0) uniform sampler2D tex_diffuse;

layout(location = 0) out vec3 position;
layout(location = 1) out vec4 albedo;
layout(location = 2) out vec3 normal;

in vec3 Position_worldspace;
in vec2 Texcoord;
in vec3 Normal_worldspace;

uniform float MaterialShininess;

vec2 encodeNormal(vec3 normal){
	return normalize(normal.xy) * sqrt(-normal.z * 0.5 + 0.5);
}

void main(){
	position = Position_worldspace;
	albedo = vec4(texture(tex_diffuse, Texcoord).rgb, MaterialShininess);
	normal = vec3(encodeNormal(normalize(Normal_worldspace)), 1.0);
	//normal = normalize(Normal_worldspace);
}