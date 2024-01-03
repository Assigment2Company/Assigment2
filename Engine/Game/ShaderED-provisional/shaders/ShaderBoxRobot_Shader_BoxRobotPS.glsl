#version 440

#define pi 3.142
#define numLights 4

in vec3 position;
in vec3 normal;
in vec2 uv0;
in vec2 uv1;

//Light properties
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float lightIntensity;

uniform vec3 cameraPos;
uniform vec3 ambientColor;

//Material properties
uniform float shininess;
uniform float indexRefraction; //Refraction of the material
uniform sampler2D diffuseTexture;
//uniform sampler2D specularTexture;

out vec4 outColor;

void main() {
	vec3 diffuseColor = vec3(texture(diffuseTexture, uv0)); //Diffuse color
	float specularColor = (texture(diffuseTexture, uv0)).b; //Diffuse color
	
	vec3 N = -normalize(normal);  //Normal
	
	vec3 L =  normalize(lightDir); //Light direction
	float NdotL = dot(N,L);
	
	vec3 R = reflect(L,N);
	vec3 V = normalize(cameraPos - position); //Vire direction
	float VdotRpown = pow(max(dot(V,R), 0), shininess);
	
	float RFOi = specularColor + (1-specularColor) * pow(1-NdotL,5);
	
	vec3 Li = lightIntensity * lightColor;  //Incoming radiance
	
	vec3 color = ((diffuseColor*(1-specularColor))/ pi + ((shininess +2) / (2*pi))* RFOi * VdotRpown) 
					  * Li * NdotL;
	
	//It doesn't make sense for color to be negative
	for(int i=0; i<3; i++){
		if(color[i] < 0){
			color[i] = 0;
		}
	}
					  				  				  
	vec3 finalColor = ambientColor * diffuseColor + color;
	
	//Output
	outColor = vec4(finalColor, 1.0f);
}
