#version 330 core
out vec4 FragColor;
uniform vec3 gcolor;
void main()
{
    FragColor = vec4(gcolor, 0.7);

} 
