#version 330 core

in vec3 Normal_cameraspace;

out vec4 FragColor;

uniform vec3 Color;

void main(){
	vec3 L = normalize(vec3(1, 1, 1));
	vec3 N = normalize(Normal_cameraspace);
	vec3 E = normalize(vec3(0, 0, 1));
	vec3 R = reflect(-L, N);

	float diffuseIntensity = clamp((dot(N, L) + 1.0) * 0.5, 0.0, 1.0);
	float specularIntensity = clamp(pow(dot(E, R), 1.0), 0.0, 1.0);

	FragColor = vec4(diffuseIntensity * Color + specularIntensity + 0.1, 1.0);
}