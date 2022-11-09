#version 330
layout (location = 0) in vec3 Position;
//layout (location = 1) in vec2 TexCoord;
layout (location = 3) in vec3 Normal;
uniform mat4 MVPMatrix;
out vec2 TexCoordOut;
void main()
{
    gl_Position = MVPMatrix * vec4(Position, 1.0);
    TexCoordOut = vec2(gl_Position.x, gl_Position.y);
}
