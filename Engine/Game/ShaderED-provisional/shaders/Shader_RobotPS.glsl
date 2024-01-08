#version 440

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

out vec4 outColor;

void main() {
	
	//Using  gamma correction forces to transform sRGB textures to linear space
	vec3 diffuseColor = pow(vec3(texture(diffuseTexture, uv0)), vec3(2.2)); //Diffuse color
	vec3 specularColor = pow(vec3(texture(specularTexture, uv0)),vec3(2.2)); //Specular map
	
	float shininess = exp2(15*texture(specularTexture, uv0).a+1);
	
	vec3 N = -normalize(normal);  	//Normal
	vec3 L =  normalize(lightDir); 	//Light direction
	float NdotL = max(dot(N,L),0);	//It doesn't make sense for color to be negative
	
	vec3 R = reflect(L,N);
	vec3 V = normalize(cameraPos - position); //View direction
	float VdotRpown = pow(max(dot(V,R), 0), shininess);
	
	vec3 RFOi = specularColor + (1-specularColor) * pow(1-NdotL,5);
	
	vec3 Li = lightIntensity * lightColor;  //Incoming radiance
	
	//Color with specular and no pi corretion
	vec3 lightColor = ((diffuseColor*(1-specularColor)) + ((shininess +2)/2)* RFOi * VdotRpown) * Li * NdotL;
				  				  				  
	vec3 color = ambientColor * diffuseColor + lightColor;
	
	//Gamma correction
	color.rgb = pow(color.rgb, vec3(1/2.2));
	
	
	//Output
	outColor = vec4(color, 1.0f);
}
