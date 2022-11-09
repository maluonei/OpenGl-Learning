#version 450 core

layout(location=0) out vec4 color;

void main(){
	color = vec4(1.0);
	//color = gl_FragDepth*vec4(1.0f);
	float exp = 15.0f;
	gl_FragDepth = gl_FragCoord.z;
	color = vec4(vec3(pow(gl_FragCoord.z, exp)), 1.0);
}