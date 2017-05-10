#version 410

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMat;
uniform sampler2D image;

in vec2 vTexCoords[];

out vec3 gNormals;
out vec2 gTexCoords;
out vec3 gCamera;
out vec3 gLight;

void main() {

	//getting position information
	vec4 position0 = gl_in[0].gl_Position;
	vec4 position1 = gl_in[1].gl_Position;
	vec4 position2 = gl_in[2].gl_Position;
	position0.y = position0.y + 2*texture(image, vTexCoords[0]).x;
	position1.y = position1.y + 2*texture(image, vTexCoords[1]).x;
	position2.y = position2.y + 2*texture(image, vTexCoords[2]).x;
	position0 = projection * view * model * position0;
	position1 = projection * view * model * position1;
	position2 = projection * view * model * position2;

	//calculate cross product
	vec4 u = position1 - position0;
	vec4 v = position2 - position0;
	
	vec3 normal;
	normal.x = (u.y * v.z) - (u.z * v.y);
	normal.y = (u.z * v.x) - (u.x * v.z);
	normal.z = (u.x * v.y) - (u.y * v.x);
	
	//setting all necessary values for each vertex
	gNormals = normalize(normalMat * vec4(normal,1.0)).xyz;
	gTexCoords = vTexCoords[0];
	vec4 eyepoint = view * model * position0;
	gLight = normalize(vec3(3,5,3) - eyepoint.xyz);
	gCamera = normalize(-eyepoint.xyz);
	gl_Position = position0;
    EmitVertex();
	
	gNormals = normalize(normalMat * vec4(normal,1.0)).xyz;
	gTexCoords = vTexCoords[1];
	eyepoint = view * model * position1;
	gLight = normalize(vec3(3,5,3) - eyepoint.xyz);
	gCamera = normalize(-eyepoint.xyz);
	gl_Position = position1;
    EmitVertex();
	
	gNormals = normalize(normalMat * vec4(normal,1.0)).xyz;
	gTexCoords = vTexCoords[2];
	eyepoint = view * model * position2;
	gLight = normalize(vec3(3,5,3) - eyepoint.xyz);
	gCamera = normalize(-eyepoint.xyz);
	gl_Position = position2;
    EmitVertex();
}