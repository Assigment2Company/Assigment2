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
	vec3 phongColor = ambientColor * diffuseColor 
	+ Kd * diffuseColor * lightColor * (NdotL) 
	+ Ks * lightColor * VdotRpown;
	outColor = vec4(phongColor, 1.0f);
}
// (diffuseColor / pi + ((n +2) / 2*pi) * VdotRpown) * Li * NdotL