#version 330 core
out vec4 fragment;

in vec4 color;
in vec2 texcoord;

uniform sampler2D u_texture;
uniform sampler2D u_texture1;

void main()
{
    vec4 tex_color = texture(u_texture, texcoord);
    fragment = color * mix(tex_color, texture(u_texture1, texcoord), tex_color.a * 0.2);
}
