#version 430 core

layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aOffset;
layout(location=2) in float aDensity;

flat out float vDensity;

uniform vec3 cellSize;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main(){
	vec3 worldPos = aOffset+aPos*cellSize;
	gl_Position=projection*view*model*vec4(worldPos, 1.0f);
	vDensity = aDensity;
}
