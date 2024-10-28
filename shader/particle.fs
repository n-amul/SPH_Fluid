#version 430 core
in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 LightDirection=normalize(vec3(-1,0,0));
uniform vec3 LightColor=vec3(1.0, 1.0, 1.0);
uniform vec3 DiffuseColor=vec3(0.0,0.5,0.9);

out vec4 finalColor;

void main() {
	// Compute irradiance (sum of ambient & direct lighting)
	vec3 irradiance= vec3(0.3,0.3,0.3) * DiffuseColor + DiffuseColor * LightColor * max(0,dot(LightDirection,normalize(fragNormal)));
	
	// Gamma correction
	finalColor=vec4(irradiance,1);
}
