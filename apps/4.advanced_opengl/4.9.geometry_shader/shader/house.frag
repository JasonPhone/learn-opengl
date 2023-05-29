#version 330 core
out vec4 fragColor;

in vec3 fColor;

void main()
{
    fragColor = vec4(fColor, 1.0);   
}
