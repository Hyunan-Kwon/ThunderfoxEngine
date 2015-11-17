#version 330 core

in vec2 UV;

out vec4 FragColor;

uniform sampler2D DepthTexture;
uniform sampler2D NormalTexture;
uniform sampler2D RenderedTexture;
uniform sampler2D PositionTexture;
uniform sampler2D PaperTexture;
uniform mat4 P;
uniform vec3 LightColor;

uniform float edge0, edge1;

const float DistanceThreshold = 0.05;
const vec2 FilterRadius = vec2(20.0/1024.0, 20.0/768.0);
const vec2 texelSize = vec2(1.0/1024.0, 1.0/768.0);

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

float calcGradient(mat4 invP, vec2 texcoord, vec2 texelsize){
	float up = backProjection(invP, vec3(texcoord, texture(DepthTexture, texcoord + vec2(0.0, -texelsize.y)).r) * 2.0 - 1.0); 
	float down = backProjection(invP, vec3(texcoord, texture(DepthTexture, texcoord + vec2(0.0, texelsize.y)).r) * 2.0 - 1.0);
	float left = backProjection(invP, vec3(texcoord, texture(DepthTexture, texcoord + vec2(-texelsize.x, 0.0)).r) * 2.0 - 1.0);
	float right = backProjection(invP, vec3(texcoord, texture(DepthTexture, texcoord + vec2(texelsize.x, 0.0)).r) * 2.0 - 1.0);

	return abs(up - down) + abs(left - right);
}

float getYColor(vec3 c){
	return c.r *  .299000 + c.g *  .587000 + c.b *  .114000;
}

float calcGradient2(mat4 invP, vec2 texcoord, vec2 texelSize){
	float up = getYColor(texture(RenderedTexture, texcoord + vec2(0.0, -texelSize.y)).rgb);
	float down = getYColor(texture(RenderedTexture, texcoord + vec2(0.0, texelSize.y)).rgb);
	float left = getYColor(texture(RenderedTexture, texcoord + vec2(-texelSize.x, 0.0)).rgb);
	float right = getYColor(texture(RenderedTexture, texcoord + vec2(texelSize.x, 0.0)).rgb);

	return abs(up - down) + abs(left - right);
}

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

	ambientOcclusion = 1.0 - ambientOcclusion / sample_count;
	ambientOcclusion = ambientOcclusion * 0.5 + 0.5;
	//ambientOcclusion = clamp(ambientOcclusion, 0.0, 1.0);

	//FragColor =  vec4(texture(RenderedTexture, UV).xyz * ambientOcclusion, 1.0);

	vec3 AO_color = vec3(1.0) - LightColor * ambientOcclusion;

	if(UV.x > 0.5){
		FragColor = vec4(texture(RenderedTexture, UV).xyz - AO_color, 1.0);
	}
	else{
		float foo = dot(normal, vec3(0, 0, 1));
		foo = 2.0 - foo;
		vec3 paper = texture(PaperTexture, foo * UV).xyz;

		float gradient = 1.0 - clamp(calcGradient2(invP, UV, texelSize), 0.0, 1.0);
		gradient = smoothstep(edge0, edge1, gradient);
		//FragColor = vec4(vec3(gradient), 1.0);
		vec3 color = gradient * texture(RenderedTexture, UV).xyz;
		color = color * paper;
		color = color * ambientOcclusion;
		FragColor = vec4(color, 1.0);
	}
}