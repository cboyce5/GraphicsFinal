
#version 430

in vec2 vTexCoords;

uniform sampler2D image;

layout(location = 0) out vec4 fragColorOut;

in vec3 gNormals;
in vec2 gTexCoords;
in vec3 gCamera;
in vec3 gLight;

const vec3 lightPos = vec3(3.0, 0.0, 3.0);
const vec3 ambientColor = vec3(0.0, 0.0, 0.1);
const vec3 diffuseColor = vec3(0.0, 0.0, 0.5);
const vec3 specColor = vec3(1.0, 1.0, 1.0);

void main() {

	//Blinn-Phong shading
	vec3 normal = normalize(gNormals);
	
	vec3 halfwayVec = normalize( gLight + gCamera );

	float spec = pow( max( dot(normal, halfwayVec), 0.0 ), 32.0);

	fragColorOut = vec4(spec,spec,spec, 1.0) + vec4(0.1,0.1,0.4,1.0);

	
}