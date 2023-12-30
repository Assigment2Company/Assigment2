#version 440
in vec3 position;
in vec3 normal;
in vec2 uv0;

uniform vec3 lightDir;
uniform vec3 cameraPos;
uniform vec3 ambientColor;
uniform vec3 lightColor;
uniform float Ks;
uniform float Kd;
uniform float shininess;
uniform sampler2D diffuseTexture;

out vec4 outColor;

void main() {
	vec3 diffuseColor = vec3(texture(diffuseTexture, uv0));
	vec3 N = -normalize(normal);
	vec3 L =  normalize(lightDir);
	float NdotL = dot(N,L);
	vec3 R = reflect(L,N);
	vec3 V = normalize(cameraPos - position);
	float VdotRpown = pow(max(dot(V,R), 0.0), shininess);
	float RF0 = pow((shininess-1)/(shininess+1), 2);
	float RFOi = RF0 + (1-RF0) * pow(1-dot(Li,N),5); // Li y N tienen que estar normalizados
	vec3 phongColor = ((diffuseColor*(1-RF0) )/ pi + ((n +2) / 2*pi)* RFOi * VdotRpown) * Li * NdotL;
	outColor = vec4(phongColor, 1.0f);
}
// RF0 = pow((n-1)/(n+1), 2)
// RFOi = RF0 + (1-RF0) * pow(1-cosOi,5)
// ((diffuseColor*(1-RF0) )/ pi + ((n +2) / 2*pi)* RFOi * VdotRpown) * Li * NdotL