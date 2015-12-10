#version 430

layout(binding = 0) uniform sampler2D tex_position;
layout(binding = 1) uniform sampler2D tex_albedo;
layout(binding = 2) uniform sampler2D tex_normal;

in vec2 UV;

out vec4 FragColor;

vec3 restoreNormal(vec2 normal){
	float z = sqrt(1.0 - normal.x * normal.x - normal.y * normal.y);
	return vec3(normal, z);
//	normalXY = normalXY * 2.0 - 1.0;
//	float z = (normalXY.x * normalXY.x + normalXY.y * normalXY.y) * 2.0 - 1.0;
//	return vec3(normalXY, z);
}

void main(){
	vec4 albedo = texture(tex_albedo, UV);
	vec3 material_diffuse = albedo.rgb;
	float material_shininess = albedo.a;

	vec3 L = vec3(0, 0, 1);
	vec3 N = restoreNormal(texture(tex_normal, UV).xy);
	vec3 E = vec3(0, 0, 1);
	vec3 R = reflect(-L, N);

	float diffuseIntensity = clamp(dot(N, L), 0.0, 1.0);
	float specularIntensity = clamp(pow(dot(E, R), material_shininess), 0.0, 1.0);

	FragColor = vec4(diffuseIntensity * material_diffuse + specularIntensity * vec3(1, 1, 1), 1.0);
}