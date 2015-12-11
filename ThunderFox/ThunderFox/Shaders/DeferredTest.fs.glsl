#version 430

layout(binding = 0) uniform sampler2D tex_position;
layout(binding = 1) uniform sampler2D tex_albedo;
layout(binding = 2) uniform sampler2D tex_normal;

in vec2 UV;

out vec4 FragColor;

uniform mat4 V;

vec3 decodeNormal(vec2 normal){
	float z = length(normal.xy) * 2.0 - 1.0;
	z = -z;
	return vec3(normalize(normal.xy) * sqrt(1.0 - z * z), z);
}

void main(){
	vec4 albedo = texture(tex_albedo, UV);
	vec3 material_diffuse = albedo.rgb;
	float material_shininess = albedo.a;

	vec3 position = texture(tex_position, UV).xyz;
	vec3 L = normalize(vec3(V * vec4(0, 1, 0, 1.0)).xyz - position);
	//vec3 N = texture(tex_normal, UV).xyz;
	vec3 N = normalize(decodeNormal(texture(tex_normal, UV).xy));
	vec3 E = normalize(vec3(0, 0, 0) - position);
	//vec3 R = reflect(-L, N);
	vec3 R = normalize(2.0 * dot(L, N) * N - L);

	float diffuseIntensity = clamp(dot(N, L), 0.0, 1.0);
	//float specularIntensity = clamp(pow(dot(E, R), material_shininess), 0.0, 1.0);
	float specularIntensity = 0.0;

	FragColor = vec4(diffuseIntensity * material_diffuse + specularIntensity * vec3(1, 1, 1), 1.0);
	//FragColor = vec4(texture(tex_normal, UV).xy, 1.0, 1.0);
		
	if(gl_FragCoord.x < 1024 / 2)
		FragColor = vec4(vec3(N.z), 1.0);
	else
		FragColor = vec4(vec3(diffuseIntensity), 1.0);
}