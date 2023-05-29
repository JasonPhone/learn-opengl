#version 330 core
layout (location = 0) in vec2 vPos;
layout (location = 1) in vec3 vColor;

out VS_OUT {
    vec3 color;
} vert_out;

void main()
{
    gl_Position = vec4(vPos.x, vPos.y, 0.0, 1.0); 
    vert_out.color = vColor;
}