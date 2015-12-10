#version 430

uniform float roll;
uniform int R;
//uniform writeonly image2D destTex;

layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, r32f) uniform image2D destTex;
//layout(binding = 0, r32f) uniform writeonly iamge2D destTex;

void main() {
	//ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	//float localCoef = length(vec2(ivec2(gl_LocalInvocationID.xy) - 8) / 8.0);
	//float globalCoef = sin(float(gl_WorkGroupID.x + gl_WorkGroupID.y) * 0.1 + roll) * 0.5;
	
	//vec4 texel = imageLoad(destTex, storePos);
	////imageStore(destTex, storePos, vec4(1.0 - globalCoef * localCoef, 0.0, 0.0, 0.0));
	//imageStore(destTex, storePos, vec4(0.0));
	int radius = 3;

	for(int i=0; i<R; ++i){
	 radius += R;
	}

	ivec2 texCoord = ivec2(gl_GlobalInvocationID.xy);

	
	vec4 sum = vec4(0.0);
	for(int x = -radius; x <= radius; ++x){
		for(int y = -radius; y <= radius; ++y){
			ivec2 sampleCoord = texCoord + ivec2(x, y);
			sum += imageLoad(destTex, sampleCoord);
		}
	}
	sum /= ((2 * radius + 1) * (2 * radius + 1));

	imageStore(destTex, texCoord, sum);
}