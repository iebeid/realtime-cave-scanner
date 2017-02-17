#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 normalInterp;
out vec3 vertPos;
out vec3 lightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    TexCoords = texCoords;
	normalInterp = vec3(normalMatrix * vec4(normal, 0.0));
	vec4 vertPos4 = view * model * vec4(position, 1.0);
	vertPos = vec3(vertPos4) / vertPos4.w;
	lightPos = vec3(0.0f,10.0f,0.0f);
}