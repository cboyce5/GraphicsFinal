
#version 430

in vec2 vTexCoords;

uniform sampler2D image;

layout(location = 0) out vec4 fragColorOut;

void main() {
	
	fragColorOut = texture(image,vTexCoords);
}