#version 330 core

in VertexData{
    vec3 Normal_cameraspace;
    vec2 UV;
    vec3 EyeDirection_cameraspace;
    vec4 ShadowCoord;
    vec4 Position_worldspace;
    vec4 Position_cameraspace;
} VertexIn;

flat in vec3 midpoint;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 Position;
layout(location = 3) out float Visibility;

uniform sampler2D Texture00;
uniform sampler2DShadow Tex_shadowMap;
uniform mat4 V;
uniform vec3 LightColor;
uniform vec3 LightDirection_worldspace;
uniform vec3 MaterialAmbient;
uniform vec3 MaterialDiffuse;
uniform vec3 MaterialSpecular;
uniform float MaterialShininess;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed, i);
	return fract(sin(dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673))) * 43758.5453);
}

void main(){
	vec3 LightDirection_cameraspace = (V * vec4(LightDirection_worldspace, 0)).xyz;

	vec4 MaterialDiffuseColor = vec4(MaterialDiffuse, 1.0) * texture(Texture00, VertexIn.UV);
	vec4 MaterialAmbientColor = vec4(MaterialAmbient, 1.0) * texture(Texture00, VertexIn.UV);
	vec4 MaterialSpecularColor = vec4(MaterialSpecular, 1.0);
	float shininess = MaterialShininess > 0? MaterialShininess : 1;

	vec3 L = normalize(LightDirection_cameraspace);
	vec3 N = normalize(VertexIn.Normal_cameraspace);
	vec3 E = normalize(VertexIn.EyeDirection_cameraspace);
	vec3 R = reflect(-L, N);

	//float diffuseIntensity = clamp((dot(N, L) + 1.0) * 0.5, 0.0, 1.0);	// Half Lambert
	float diffuseIntensity = clamp(dot(N, L), 0.0, 1.0);
	float specularIntensity = clamp(pow(dot(E, R), shininess), 0.0, 1.0);

	//float visibility = 1.0;
	//float bias = 0.005 * tan(acos(clamp(dot(N, L), 0.0, 1.0)));
	//bias = clamp(bias, 0.0, 0.005);
	//bias = 0;

	//float shadow = textureProj(Tex_shadowMap, ShadowCoord, bias);
	//float voxel = (ShadowCoord.z - bias) / ShadowCoord.w;
	//if(shadow < voxel){
	//	visibility = 0.2;
	//}
	//for(int i=0; i<5; ++i){
	//	//if(textureProj(Tex_shadowMap, vec4(ShadowCoord.xy + poissonDisk[i]/700.0, ShadowCoord.z, 1.0), bias) < ShadowCoord.z - bias){
	//	//	visibility -= 0.1f;
	//	//}
	//	//int index = int(16.0 * random(gl_FragCoord.xyy, i)) % 16;
	//	int index = int(16.0 * random(floor(Position_worldspace.xyz * 1000.0), i)) % 16;
	//	vec2 shift = poissonDisk[index]/2000.0;
	//	if(textureProj(Tex_shadowMap, vec4(ShadowCoord.xy + shift, ShadowCoord.z, 1.0), bias) < ShadowCoord.z - bias){
	//		visibility -= 0.2f;
	//	}
	//	//visibility -= 0.1 * (1.0 - textureProj(Tex_shadowMap, vec4(ShadowCoord.xy + poissonDisk[index]/700.0, (ShadowCoord.z - bias)/ShadowCoord.w, 1), 1));
	//}

	//Visibility = visibility;
	
	//Visibility = min(diffuseIntensity, visibility);
	//diffuseIntensity = Visibility;

	FragColor = MaterialAmbientColor
		+ diffuseIntensity * MaterialDiffuseColor * vec4(LightColor, 1.0)
		+ specularIntensity * MaterialSpecularColor * vec4(LightColor, 1.0);

	Normal = (N + 1.0) * 0.5;
	//Position = VertexIn.Position_cameraspace.xyz;

	//midpoint = P *vec4(midpoint, 1.0);
	//Position = vec3(midpoint.xy, 0.0);
	Position = midpoint.xy;
	//Position = vec2((gl_FragCoord.x - 0.5) / 1024.0, (gl_FragCoord.y - 0.5) / 768.0);
	//FragColor = vec4(midpoint.xy, 0.0, 1.0);
} 