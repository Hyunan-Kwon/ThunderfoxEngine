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

void main(){
	midpoint = vec3(0.0);
	for(int i = 0; i < gl_in.length(); ++i){
		gl_Position = gl_in[i].gl_Position;

		VertexOut.Normal_cameraspace = VertexIn[i].Normal_cameraspace;
        VertexOut.UV = VertexIn[i].UV;
        VertexOut.EyeDirection_cameraspace = VertexIn[i].EyeDirection_cameraspace;
        VertexOut.ShadowCoord = VertexIn[i].ShadowCoord;
		VertexOut.Position_worldspace = VertexIn[i].Position_worldspace;
		VertexOut.Position_cameraspace = VertexIn[i].Position_cameraspace;

        midpoint += gl_in[i].gl_Position.xyz;
		EmitVertex();
	}
	midpoint /= 3;
	EndPrimitive();	
}