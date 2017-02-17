#version 330 core

in vec2 TexCoords;
in vec3 normalInterp;
in vec3 vertPos;
in vec3 lightPos;

out vec4 color;

uniform sampler2D texture_diffuse1;

void main()
{    
	vec3 ambientColor = vec3(0.1, 0.1, 0.1);
	vec4 diffuseColor = texture(texture_diffuse1, TexCoords);
	vec3 specColor = vec3(0.5, 0.5, 0.5);
	vec3 normal = normalize(normalInterp);
	vec3 lightDir = normalize(lightPos - vertPos);
	float lambertian = max(dot(lightDir,normal), 0.0);
	float specular = 1.0;
    vec3 viewDir = normalize(-vertPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float specAngle = max(dot(halfDir, normal), 0.0);
	const float shininess = 100.0;
    specular = pow(specAngle, shininess);
	const float screenGamma = 0.8;
	vec3 colorLinear = ambientColor + lambertian * diffuseColor.xyz + specular * specColor;
	vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0/screenGamma));
	color = vec4(colorGammaCorrected, 1.0);
}