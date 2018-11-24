#version 330 core
uniform mat4 model_view_projection;
uniform mat4 model_view;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec2 in_texcoord;

out vec4 color;
out vec2 texcoord;

void main()
{
    vec4 position = model_view_projection * vec4(in_position, 1.0);
    /*position.xyz = position.xyz / position.w;
    position.x = floor(200.0 * position.x) / 200.0;
    position.y = floor(200.0 * position.y) / 200.0;
    position.xyz *= position.w;*/

    float distance = length(model_view * position);

    gl_Position = position;
    color = in_color;
    texcoord = in_texcoord * (distance + position.w) / (distance);
}
