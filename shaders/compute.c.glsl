
#version 430

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

uniform int imageSize;
uniform float time;
uniform int control;
uniform float random;
uniform int toggleR;

//random value generator
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {

	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	
	vec4 pixel;
	// base pixel colour for image
	
	//set a bunch of different sin waves
	float x_sin = sin(0.1*(time+(pixel_coords.x * rand(vec2(random, 0.5)))))*0.5 + 0.5;
	float y_sin = sin(0.1*(time+(pixel_coords.y * rand(vec2(random, 0.8)))))*0.5 + 0.5;
	float w_sin = sin(0.1*(time+(pixel_coords.y + pixel_coords.x)*rand(vec2(0.1, random))))*0.5 + 0.5;
	float a_sin = sin(0.9*(time+pixel_coords.y*rand(vec2(0.4, random))))*0.01 + 0.5;
	
	float b_sin = sin(0.1*(time + pixel_coords.x * rand(vec2(random, 0.34)) + pixel_coords.y * rand(vec2(random, 0.55))))*0.2 + 0.5;
	float c_sin = sin(0.1*(time+(pixel_coords.y * rand(vec2(random, 0.7))) + pixel_coords.x * rand(vec2(random, 0.82))))*0.2 + 0.5;
	float d_sin = sin(0.1*(time+(pixel_coords.y + pixel_coords.x)*rand(vec2(0.4, random))))*0.2 + 0.5;
	float e_sin = sin(0.9*(time+pixel_coords.y*rand(vec2(0.9, random)) + pixel_coords.x * rand(vec2(0.77, random))))*0.01 + 0.5;
	
	//toggling random value
	float r;
	if (toggleR == 0) 
		r = mod(rand(vec2(pixel_coords.x, pixel_coords.y)),0.1);
	else 
		r = 0;
	
	//selecting different waves
	if (control == 0)
		pixel = vec4( x_sin + w_sin + r, 0.0, 0.0, 1.0);
	else if (control == 1)
		pixel = vec4( x_sin + r, 0.0, 0.0, 1.0);
	else if (control == 2)
		pixel = vec4( y_sin + r, 0.0, 0.0, 1.0);
	else if (control == 3)
		pixel = vec4( x_sin + y_sin + r, 0.0, 0.0, 1.0);
	else if (control == 4)
		pixel = vec4( a_sin + r, 0.0, 0.0, 1.0);
	else if (control == 5)
		pixel = vec4(x_sin + y_sin + w_sin + a_sin + b_sin + c_sin + d_sin + e_sin + r, 0.0, 0.0, 1.0);
	else 
		pixel = vec4(0.0,0.0,0.0,1.0);
		


	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}