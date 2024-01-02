#version 440
in vec3 position;
in vec3 normal;
in vec2 uv0;

uniform vec3 lightDir;
uniform vec3 cameraPos;
uniform vec3 ambientColor;
uniform vec3 lightColor;
uniform float shininess;
uniform float indexRefraction; //Refraction of the material
uniform float Li; //Incoming radiance
uniform sampler2D diffuseTexture;

out vec4 outColor;

void main() {
	vec3 diffuseColor = vec3(texture(diffuseTexture, uv0)); //Diffuse color
	
	vec3 N = -normalize(normal);  //Normal
	vec3 L =  normalize(lightDir); //Light direction
	float NdotL = dot(N,L);
	
	vec3 R = reflect(L,N);
	vec3 V = normalize(cameraPos - position); //Vire direction
	float VdotRpown = pow(max(dot(V,R), 0.0), shininess);
	
	float RF0 = pow((indexRefraction-1)/(indexRefraction+1), 2);
	float RFOi = RF0 + (1-RF0) * pow(1-NdotL,5);
	
	vec3 phongColor = ((diffuseColor*(1-RF0))/ 3.142 + ((shininess +2) / 2*3.142)* RFOi * VdotRpown) * lightColor* Li * NdotL;
	
	//Output
	outColor = vec4(phongColor, 1.0f);
}
