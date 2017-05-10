
#version 430

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec2 texCoords;

out vec2 vTexCoords;

void main() {
    gl_Position = projection * view * model * vertexPosition;
    
    vTexCoords =  texCoords;
}