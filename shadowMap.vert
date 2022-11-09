#version 450 core

uniform mat4 MVPMatrix;
layout(location=0) in vec4 vPosition;

void main(){
	gl_Position = MVPMatrix * vPosition;
}