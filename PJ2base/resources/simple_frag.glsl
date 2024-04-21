#version 330 core 
in vec3 fragNor;
in vec3 vertPos;
in vec3 fragPos;
out vec4 color;
uniform vec3 lightP;
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;

void main()
{	
	vec3 lightDir = lightP - fragPos;
	float distance = length(lightDir);

	distance=distance/3;

	lightDir=normalize(lightDir);

	vec3 normal = normalize(fragNor);

	// fix these
	float dc =max(dot(lightDir, normal), 0.0);
	float sc=0.0;

	if (dc > 0.0) {
    vec3 viewDir = normalize(fragPos);
    vec3 halfV = normalize(lightDir + viewDir);
	sc = pow(max(dot(halfV, normal), 0.0),MatShine);
    }

	color = vec4((MatAmb+(MatDif*dc/distance)+(MatSpec*sc/distance)),1);

}
