#version 330
in vec2 TexCoordOut;
uniform sampler2D depth_texture;
out vec4 FragColor;
void main()
{
    float Depth = texture(depth_texture, TexCoordOut).x;
    Depth = 1.0 - (1.0 - Depth) * 25.0;
    FragColor = vec4(Depth);
}
