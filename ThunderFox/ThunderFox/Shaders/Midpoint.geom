#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData{
    vec3 Normal_cameraspace;
    vec2 UV;
    vec3 EyeDirection_cameraspace;
    vec4 ShadowCoord;
    vec4 Position_worldspace;
    vec4 Position_cameraspace;
} VertexIn[];

out VertexData{
    vec3 Normal_cameraspace;
    vec2 UV;
    vec3 EyeDirection_cameraspace;
    vec4 ShadowCoord;
    vec4 Position_worldspace;
    vec4 Position_cameraspace;
} VertexOut;

flat out vec3 midpoint;

uniform mat4 P;

void main(){
	vec3 _mp = vec3(0.0);
	for(int i = 0; i < gl_in.length(); ++i){
		_mp += VertexIn[i].Position_cameraspace.xyz;
	}
	_mp /= 3.0;
	vec4 mp = P * vec4(_mp, 1.0);
	mp /= mp.w;
	mp = (mp + 1.0) * 0.5;

	for(int i = 0; i < gl_in.length(); ++i){
		gl_Position = gl_in[i].gl_Position;

		VertexOut.Normal_cameraspace = VertexIn[i].Normal_cameraspace;
        VertexOut.UV = VertexIn[i].UV;
        VertexOut.EyeDirection_cameraspace = VertexIn[i].EyeDirection_cameraspace;
        VertexOut.ShadowCoord = VertexIn[i].ShadowCoord;
		VertexOut.Position_worldspace = VertexIn[i].Position_worldspace;
		VertexOut.Position_cameraspace = VertexIn[i].Position_cameraspace;
		midpoint = mp.xyz;
		//midpoint = vec3(clamp(_mp.x, 0.0, 1.0), clamp(_mp.y, 0.0, 1.0), 0.0);

		EmitVertex();
	}
	EndPrimitive();	
}