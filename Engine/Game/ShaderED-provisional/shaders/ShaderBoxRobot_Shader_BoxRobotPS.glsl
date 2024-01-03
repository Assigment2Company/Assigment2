#version 440

#define pi 3.142

in vec3 position;
in vec3 normal;
in vec2 uv0;

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
uniform sampler2D specularTexture;

out vec4 outColor;

void main() {
	vec3 diffuseColor = vec3(texture(diffuseTexture, uv0)); //Diffuse color
	float specularColor = (texture(specularTexture, uv0)).r; //Specular map
	
	vec3 N = -normalize(normal);  //Normal
	vec3 L =  normalize(lightDir); //Light direction
	float NdotL = dot(N,L);
	
	vec3 R = reflect(L,N);
	vec3 V = normalize(cameraPos - position); //View direction
	float VdotRpown = pow(max(dot(V,R), 0), shininess);
	
	
	float RF0 = pow((indexRefraction-1)/(indexRefraction+1), 2);//Old
	float RFOi = RF0 + (1-RF0) * pow(1-NdotL,5);				//Old
	
	float RFOi_specular = specularColor + (1-specularColor) * pow(1-NdotL,5);
	
	vec3 Li = lightIntensity * lightColor;  //Incoming radiance
	
	//Old function (used to compare to new one)
	vec3 color = ((diffuseColor*(1-RF0))/ pi + ((shininess +2) / (2*pi))* RFOi * VdotRpown) * Li * NdotL;			  
	//New color with specular map
	vec3 colorSpecular = ((diffuseColor*(1-specularColor))/ pi + ((shininess +2) / (2*pi))* RFOi_specular * VdotRpown) * Li * NdotL;
	//New color with specular and no pi corretion
	vec3 colorWithSpecularNoPi = ((diffuseColor*(1-specularColor)) + ((shininess +2))* RFOi_specular * VdotRpown) * Li * NdotL;
	//Deliting the pi corrections is in the power point although
	//I don't understand it's mathematical meaning. If variables 
	//are tweaked, the result is similar and can look good.
	
	
	//It doesn't make sense for color to be negative
	for(int i=0; i<3; i++){
		if(color[i] < 0){
			color[i] = 0;
		}
		if(colorSpecular[i] < 0){
			colorSpecular[i] = 0;
		}
		if(colorWithSpecularNoPi[i] < 0){
			colorWithSpecularNoPi[i] = 0;
		}
	}
					  				  				  
	vec3 finalColor = ambientColor * diffuseColor + colorWithSpecularNoPi;
	
	//Output
	outColor = vec4(finalColor, 1.0f);
}
