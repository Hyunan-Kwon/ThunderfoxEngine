#version 330 core

in vec2 UV;

out vec4 FragColor;

uniform sampler2D DepthTexture;
uniform sampler2D NormalTexture;
uniform sampler2D RenderedTexture;
uniform sampler2D PositionTexture;
uniform mat4 P;
const float DistanceThreshold = 0.05;
const vec2 FilterRadius = vec2(20.0/1024.0, 20.0/768.0);

const int sample_count = 16;
const vec2 poisson16[] = vec2[](    // These are the Poisson Disk Samples
                                vec2( -0.94201624,  -0.39906216 ),
                                vec2(  0.94558609,  -0.76890725 ),
                                vec2( -0.094184101, -0.92938870 ),
                                vec2(  0.34495938,   0.29387760 ),
                                vec2( -0.91588581,   0.45771432 ),
                                vec2( -0.81544232,  -0.87912464 ),
                                vec2( -0.38277543,   0.27676845 ),
                                vec2(  0.97484398,   0.75648379 ),
                                vec2(  0.44323325,  -0.97511554 ),
                                vec2(  0.53742981,  -0.47373420 ),
                                vec2( -0.26496911,  -0.41893023 ),
                                vec2(  0.79197514,   0.19090188 ),
                                vec2( -0.24188840,   0.99706507 ),
                                vec2( -0.81409955,   0.91437590 ),
                                vec2(  0.19984126,   0.78641367 ),
                                vec2(  0.14383161,  -0.14100790 )
                               );


vec3 backProjection(mat4 invP, vec3 p_ndc){
	float w_clip = P[3][2] / (P[2][2] + p_ndc.z);
	vec4 p_clip = w_clip * vec4(p_ndc, 1.0);
	return (invP * p_clip).xyz;
}

//vec3 decodeNormal(vec2 normalXY){
//	normalXY = normalXY * 2.0 - 1.0;
//	float z = (normalXY.x * normalXY.x + normalXY.y * normalXY.y) * 2.0 - 1.0;
//	return vec3(normalXY, z);
//}

void main(){
	float depth = texture(DepthTexture, UV).r;
	//vec2 normalXY = texture(NormalTexture, UV).xy;
	//vec3 normal = decodeNormal(normalXY);
	vec3 normal = texture(NormalTexture, UV).xyz * 2.0 - 1.0;
	//if(normal.z < 0.0)
	//	discard;
	//vec3 pos = decodePosition(UV, depth);

	mat4 invP = inverse(P);

	vec3 pos = backProjection(invP, vec3(UV, texture(DepthTexture, UV).r) * 2.0 - 1.0);

	float ambientOcclusion = 0;
	for(int i = 0; i < sample_count; ++i){
		vec2 sampleTexcoord = UV + (poisson16[i] * FilterRadius);
		//float sampleDepth = texture(DepthTexture, sampleTexcoord).r;
		//vec3 samplePos = decodePosition(sampleTexcoord, sampleDepth);
		vec3 samplePos = backProjection(invP, vec3(sampleTexcoord, texture(DepthTexture, sampleTexcoord).r) * 2.0 - 1.0);

		vec3 S = normalize(samplePos - pos);
		float NdotS = max(dot(normal, S), 0.0);
		float dist = distance(pos, samplePos);

		float a = 1.0 - smoothstep(DistanceThreshold, DistanceThreshold*2, dist);
		//float a = 1.0 - dist;
		float b = NdotS;

		ambientOcclusion += (a * b);
	}

	ambientOcclusion = 1.0 - 2 * ambientOcclusion / sample_count;
	//ambientOcclusion = clamp(ambientOcclusion, 0.0, 1.0);

	//FragColor =  vec4(texture(RenderedTexture, UV).xyz * ambientOcclusion, 1.0);
	if(UV.x > 0.5){
	FragColor = vec4(ambientOcclusion * texture(RenderedTexture, UV).xyz, 1.0);
	}
	else{
	FragColor = texture(RenderedTexture, UV);
	}
	//FragColor = vec4(pos, 1.0);

	//FragColor = texture(RenderedTexture, UV);
}