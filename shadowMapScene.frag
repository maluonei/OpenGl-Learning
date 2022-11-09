#version 450 core

//uniform sampler2D depth_texture;
uniform sampler2DShadow depth_texture;
uniform vec3 light_position;

uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_specular_power;

layout (location = 0) out vec4 color;

in VS_FS_INTERFACE
{
    vec4 shadow_coord;
    vec3 world_coord;
    vec3 eye_coord;
    vec3 normal;
    vec3 color;
} fragment;

void main(void)
{
    vec3 N = fragment.normal;
    vec3 L = normalize(light_position - fragment.world_coord);
    float LdotN = dot(N, L);
    vec3 R = reflect(-L, N);

    float diffuse = max(LdotN, 0.0);
    float specular = max(pow(dot(normalize(-fragment.eye_coord), R), material_specular_power), 0.0);

    //vec3 tempCoord = fragment.shadow_coord.xyz/fragment.shadow_coord.w;
    //vec4 c2 = texture(depth_texture, tempCoord.xy);
    //float f = float(c2.z > tempCoord.z);
    float f = textureProj(depth_texture, fragment.shadow_coord);

    color = vec4(fragment.color * material_ambient + f * (material_diffuse * diffuse + material_specular * specular), 1.0);
    //color = f*vec4(1.0f);
}
