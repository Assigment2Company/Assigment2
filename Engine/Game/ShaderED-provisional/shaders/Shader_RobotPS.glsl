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
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

out vec4 outColor;

void main() {
	vec3 diffuseColor = vec3(texture(diffuseTexture, uv0)); //Diffuse color
	float specularColor = (texture(specularTexture, uv0)).r; //Specular map
	
	float shininess = exp2(15*texture(specularTexture, uv0).a+1);
	
	vec3 N = -normalize(normal);  //Normal
	vec3 L =  normalize(lightDir); //Light direction
	float NdotL = dot(N,L);
	
	vec3 R = reflect(L,N);
	vec3 V = normalize(cameraPos - position); //View direction
	float VdotRpown = pow(max(dot(V,R), 0), shininess);
	
	float RFOi_specular = specularColor + (1-specularColor) * pow(1-NdotL,5);
	
	vec3 Li = lightIntensity * lightColor;  //Incoming radiance
	
	//Color with specular map
	vec3 colorSpecular = ((diffuseColor*(1-specularColor))/ pi + ((shininess +2) / (2*pi))* RFOi_specular * VdotRpown) * Li * NdotL;
	//Color with specular and no pi corretion
	vec3 colorSpecularNoPi = ((diffuseColor*(1-specularColor)) + ((shininess +2))* RFOi_specular * VdotRpown) * Li * NdotL;
	
	
	//I DON'T KNOW IF THIS IS RIGHT
	
	/*It doesn't make sense for color to be negative
	for(int i=0; i<3; i++){
		if(colorSpecular[i] < 0){
			colorSpecular[i] = 0;
		}
		if(colorSpecularNoPi[i] < 0){
			colorSpecularNoPi[i] = 0;
		}
	}*/
					  				  				  
	vec3 finalColor = ambientColor * diffuseColor + colorSpecularNoPi;
	
	//Output
	outColor = vec4(finalColor, 1.0f);
}
