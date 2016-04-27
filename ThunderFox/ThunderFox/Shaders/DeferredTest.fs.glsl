#version 430

layout(binding = 0) uniform sampler2DMS tex_position;
layout(binding = 1) uniform sampler2DMS tex_albedo;
layout(binding = 2) uniform sampler2DMS tex_normal;

in vec2 UV;

out vec4 FragColor;

uniform mat4 V;
uniform vec2 viewport;
uniform int nSamples;

vec3 decodeNormal(vec2 normal){
	float z = length(normal.xy) * 2.0 - 1.0;
	z = -z;
	return vec3(normalize(normal.xy) * sqrt(1.0 - z * z), z);
}

vec3 calcSample(int n, ivec2 imageCoord){
	vec4 albedo = texelFetch(tex_albedo, imageCoord, n);
	vec3 material_diffuse = albedo.rgb;
	float material_shininess = albedo.a;

	vec3 position = texelFetch(tex_position, imageCoord, n).xyz;
	vec3 L = normalize(vec3(V * vec4(0, 1, 0, 1.0)).xyz - position);
	//vec3 N = texture(tex_normal, UV).xyz;
	vec3 N = normalize(decodeNormal(texelFetch(tex_normal, imageCoord, n).xy));
	vec3 E = normalize(vec3(0, 0, 0) - position);
	//vec3 R = reflect(-L, N);
	vec3 R = normalize(2.0 * dot(L, N) * N - L);

	float diffuseIntensity = clamp(dot(N, L), 0.0, 1.0);
	//float specularIntensity = clamp(pow(dot(E, R), material_shininess), 0.0, 1.0);
	float specularIntensity = 0.0;

	return diffuseIntensity * material_diffuse + specularIntensity * vec3(1.0);
}

void main(){
	vec3 result = vec3(0.0);
	for(int i=0; i<nSamples; ++i){
		result += calcSample(i, ivec2(UV * viewport));
	}
	result /= 4;
	FragColor = vec4(result, 1.0);
}