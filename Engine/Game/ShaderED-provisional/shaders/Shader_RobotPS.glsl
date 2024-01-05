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
uniform sampler2D diffuseTexture; 	//Bind (0)
uniform sampler2D specularTexture; 	//Bind (1)
uniform sampler2D normalTexture; 	//Bind (2)	Not implemented yet

out vec4 outColor;

void main() {
	//No gamma correction
	//vec3 diffuseColor = vec3(texture(diffuseTexture, uv0));
	//float specularColor = (texture(specularTexture, uv0)).r;
	
	//Using  gamma correction forces to transform sRGB textures to linear space
	vec3 diffuseColor = pow(vec3(texture(diffuseTexture, uv0)), vec3(2.2)); //Diffuse color
	float specularColor = pow((texture(specularTexture, uv0)).r,2.2); //Specular map
	
	float shininess = exp2(15*texture(specularTexture, uv0).a+1);
	
	vec3 N = -normalize(normal);  	//Normal
	vec3 L =  normalize(lightDir); 	//Light direction
	float NdotL = max(dot(N,L),0);	//It doesn't make sense for color to be negative
	
	vec3 R = reflect(L,N);
	vec3 V = normalize(cameraPos - position); //View direction
	float VdotRpown = pow(max(dot(V,R), 0), shininess);
	
	float RFOi_specular = specularColor + (1-specularColor) * pow(1-NdotL,5);
	
	vec3 Li = lightIntensity * lightColor;  //Incoming radiance
	
	//Color with specular and no pi corretion
	vec3 lightColor = ((diffuseColor*(1-specularColor)) + ((shininess +2)/2)* RFOi_specular * VdotRpown) * Li * NdotL;
	
	//Light gamma correction
	lightColor.rgb = pow(lightColor.rgb, vec3(1/2.2));
					  				  				  
	vec3 color = ambientColor * diffuseColor + lightColor;
	
	//Output
	outColor = vec4(color, 1.0f);
}
