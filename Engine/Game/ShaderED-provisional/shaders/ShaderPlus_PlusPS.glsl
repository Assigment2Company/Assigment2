#version 440

#define pi 3.142
#define numLights 4

in vec3 position;
in vec3 normal;
in vec2 uv0;

//Light properties
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float lightIntensity;
//Light 2 properties
uniform vec3 lightDir2;
uniform vec3 lightColor2;
uniform float lightIntensity2;

uniform vec3 cameraPos;
uniform vec3 ambientColor;

//Material properties
uniform float shininess;
uniform float indexRefraction; //Refraction of the material
uniform sampler2D diffuseTexture;

out vec4 outColor;

void main() {
	vec3 diffuseColor = vec3(texture(diffuseTexture, uv0)); //Diffuse color
	
	vec3 N = -normalize(normal);  //Normal
	
	vec3 L1 =  normalize(lightDir); //Light direction
	vec3 L2 =  normalize(lightDir2); //Light direction
	float NdotL1 = dot(N,L1);
	float NdotL2 = dot(N,L2);
	
	vec3 R1 = reflect(L1,N);
	vec3 R2 = reflect(L2,N);
	vec3 V = normalize(cameraPos - position); //Vire direction
	float VdotRpown1 = pow(max(dot(V,R1), 0), shininess);
	float VdotRpown2 = pow(max(dot(V,R2), 0), shininess);
	
	float RF0 = pow((indexRefraction-1)/(indexRefraction+1), 2);
	float RFOi1 = RF0 + (1-RF0) * pow(1-NdotL1,5);
	float RFOi2 = RF0 + (1-RF0) * pow(1-NdotL2,5);
	
	vec3 Li1 = lightIntensity * lightColor;  //Incoming radiance
	vec3 Li2 = lightIntensity2 * lightColor2;  //Incoming radiance
	
	vec3 color1 = ((diffuseColor*(1-RF0))/ pi + ((shininess +2) / (2*pi))* RFOi1 * VdotRpown1) 
					  * Li1 * NdotL1;
					  
	vec3 color2 = ((diffuseColor*(1-RF0))/ pi + ((shininess +2) / (2*pi))* RFOi2 * VdotRpown2) 
					  * Li2 * NdotL2;
	
	//It doesn't make sense for color to be negative
	for(int i=0; i<3; i++){
		if(color1[i] < 0){
			color1[i] = 0;
		}
		if(color2[i] < 0){
			color2[i] = 0;
		}
	}
					  				  				  
	vec3 finalColor = ambientColor * diffuseColor + color1 + color2;
	
	//Output
	outColor = vec4(finalColor, 1.0f);
}
