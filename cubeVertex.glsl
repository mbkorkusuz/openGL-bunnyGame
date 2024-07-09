#version 330 core

vec3 I = vec3(1, 1, 1);          // point light intensity
vec3 Iamb = vec3(0.3, 0.3, 0.3); // ambient light intensity
vec3 kd = vec3(0.8, 0.6, 0.2);     // diffuse reflectance coefficient
vec3 ka = vec3(0.3, 0.3, 0.3);   // ambient reflectance coefficient
vec3 ks = vec3(0.6, 0.3, 0.2);   // specular reflectance coefficient
vec3 lightPos = vec3(2, 5, 5);   // light position in world coordinates


uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;
uniform vec3 eyePos;

uniform int yellowCube;

layout(location=0) in vec3 inVertex;
layout(location=1) in vec3 inNormal;

out vec4 color;

void main(void)
{
	// yellow cube
	if(yellowCube == 1)
	{
		I = vec3(1.4, 1.4, 1.4);          // point light intensity
	 	Iamb = vec3(0.3, 0.3, 0.3); // ambient light intensity
	 	kd = vec3(0.8, 0.8, 0.2);     // diffuse reflectance coefficient
	 	ka = vec3(0.3, 0.3, 0.3);   // ambient reflectance coefficient
	 	ks = vec3(0.6, 0.6, 0.2);   // specular reflectance coefficient
	 	lightPos = vec3(0, 5, 0);   // light position in world coordinates
	}
	//red cubes
	else if(yellowCube == 0)
	{	
		I = vec3(1.4, 1.4, 1.4);          // point light intensity
	 	Iamb = vec3(0.3, 0.3, 0.3); // ambient light intensity
	 	kd = vec3(0.8, 0.2, 0.2);     // diffuse reflectance coefficient
	 	ka = vec3(0.3, 0.3, 0.3);   // ambient reflectance coefficient
	 	ks = vec3(0.6, 0.3, 0.2);   // specular reflectance coefficient
	 	lightPos = vec3(0, 5, 0);   // light position in world coordinates	
	}

	// road
	else if(yellowCube == 3)
	{
		I = vec3(1.4, 1.4, 1.4);          // point light intensity
	 	Iamb = vec3(0.3, 0.3, 0.3); // ambient light intensity
	 	kd = vec3(0.294, 0.498, 0.898);     // diffuse reflectance coefficient
	 	ka = vec3(0.3, 0.3, 0.3);   // ambient reflectance coefficient
	 	ks = vec3(0.3, 0.5, 0.9);   // specular reflectance coefficient
	 	lightPos = vec3(0, 5, 0);   // light position in world coordinates
	}
	else if(yellowCube == 4)
	{
		I = vec3(1.4, 1.4, 1.4);          // point light intensity
	 	Iamb = vec3(0.3, 0.3, 0.3); // ambient light intensity
	 	kd = vec3(0, 0, 0.196);     // diffuse reflectance coefficient
	 	ka = vec3(0.3, 0.3, 0.3);   // ambient reflectance coefficient
	 	ks = vec3(0.6, 0.3, 0.2);   // specular reflectance coefficient
	 	lightPos = vec3(0, 5, 0);   // light position in world coordinates
	}
	 
	vec4 pWorld = modelingMatrix * vec4(inVertex, 1);
	vec3 nWorld = inverse(transpose(mat3x3(modelingMatrix))) * inNormal;

	// Compute lighting. We assume lightPos and eyePos are in world
	// coordinates.

	vec3 L = normalize(lightPos - vec3(pWorld));
	vec3 V = normalize(eyePos - vec3(pWorld));
	vec3 H = normalize(L + V);
	vec3 N = normalize(nWorld);

	float NdotL = dot(N, L); // for diffuse component
	float NdotH = dot(N, H); // for specular component

	vec3 diffuseColor = I * kd * max(0, NdotL);
	vec3 specularColor = I * ks * pow(max(0, NdotH), 100);
	vec3 ambientColor = Iamb * ka;

	// We update the front color of the vertex. This value will be sent
	// to the fragment shader after it is interpolated at every fragment.
	// Front color specifies the color of a vertex for a front facing
	// primitive.

	color = vec4(diffuseColor + specularColor + ambientColor, 1);

	// Transform the vertex with the product of the projection, viewing, and
	// modeling matrices.

    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1);
}

