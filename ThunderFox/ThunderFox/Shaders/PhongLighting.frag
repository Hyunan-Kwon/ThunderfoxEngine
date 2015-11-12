#version 330 core

in VertexData{
	in vec3 Position_worldspace;
	in vec2 UV;
	in vec3 Normal_cameraspace;
	in vec3 EyeDirection_cameraspace;
	in vec3 LightDirection_cameraspace;
	flat in bool Duplication;
} VertexIn;

out vec4 color;

uniform sampler2D myTextureSampler;
uniform mat4 M, V;
uniform vec3 LightPosition_worldspace;
uniform vec3 LightColor;
uniform float LightPower;

void main(){
	if(VertexIn.Duplication){
		color = vec4(texture2D(myTextureSampler, VertexIn.UV).rgb - vec3(0.3, 0.3, 0.3), 1.0);
		//color = vec4(0, 0, 0, 1);
	}
	else{
		vec3 MaterialDiffuseColor = texture2D(myTextureSampler, VertexIn.UV).rgb;
		vec3 MaterialAmbientColor = vec3(0.2, 0.2, 0.2) * MaterialDiffuseColor;
		vec3 MaterialSpecularColor = vec3(0.3, 0.3, 0.3);

		float distance = length(LightPosition_worldspace - VertexIn.Position_worldspace);

		vec3 n = normalize(VertexIn.Normal_cameraspace);
		vec3 l = normalize(VertexIn.LightDirection_cameraspace);
		float cosTheta = clamp(dot(n, l), 0.0f, 1.0f);
	
		vec3 E = normalize(VertexIn.EyeDirection_cameraspace);
		vec3 R = reflect(-l, n);
		float cosAlpha = clamp(dot(E, R), 0.0f, 1.0f);
	
		vec3 FragmentColor = MaterialAmbientColor +
				MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance * distance) +
				MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha, 5) / (distance * distance);
		//color = vec4(FragmentColor, 1.0);
		color = vec4(MaterialDiffuseColor, 1);
	}
}