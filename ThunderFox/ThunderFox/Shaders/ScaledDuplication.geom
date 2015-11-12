#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 9) out;

in VertexData{
	in vec3 Position_worldspace;
	in vec2 UV;
	in vec3 Normal_cameraspace;
	in vec3 EyeDirection_cameraspace;
	in vec3 LightDirection_cameraspace;
} VertexIn[3];

out VertexData{
	out vec3 Position_worldspace;
	out vec2 UV;
	out vec3 Normal_cameraspace;
	out vec3 EyeDirection_cameraspace;
	out vec3 LightDirection_cameraspace;
	flat out bool Duplication;
} VertexOut;

uniform mat4 P;
uniform float DuplicationShift;

void main(){
	for(int i = gl_in.length() - 1; i >= 0; --i){
		vec4 N = DuplicationShift * normalize(P * vec4(VertexIn[i].Normal_cameraspace, 0));
		mat4 T = mat4(1.0f);
		T[3][0] = N.x;
		T[3][1] = N.y;
		T[3][2] = N.z;
		gl_Position = T * gl_in[i].gl_Position;
		VertexOut.Position_worldspace = VertexIn[i].Position_worldspace;
		VertexOut.UV = VertexIn[i].UV;
		VertexOut.Normal_cameraspace = VertexIn[i].Normal_cameraspace;
		VertexOut.EyeDirection_cameraspace = VertexIn[i].EyeDirection_cameraspace;
		VertexOut.LightDirection_cameraspace = VertexIn[i].LightDirection_cameraspace;
		VertexOut.Duplication = true;
		EmitVertex();
	}
	EndPrimitive();

	for(int i=0; i<gl_in.length(); ++i){
		gl_Position = gl_in[i].gl_Position;
		VertexOut.Position_worldspace = VertexIn[i].Position_worldspace;
		VertexOut.UV = VertexIn[i].UV;
		VertexOut.Normal_cameraspace = VertexIn[i].Normal_cameraspace;
		VertexOut.EyeDirection_cameraspace = VertexIn[i].EyeDirection_cameraspace;
		VertexOut.LightDirection_cameraspace = VertexIn[i].LightDirection_cameraspace;
		VertexOut.Duplication = false;
		EmitVertex();
	}
	EndPrimitive();
}