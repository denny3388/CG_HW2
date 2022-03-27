#version 430
//// TODO ////
//
// Hint:
// 1. Recieve texcoord and Normal from vertex shader
// 2. Calculate and return final color to opengl
//

uniform sampler2D Texture;
uniform float transparency;
uniform float r;
uniform float g;
uniform float b;

in vec2 texcoord;
in vec3 normal;
out vec4 frag_color;

void main()
{
    frag_color = texture2D(Texture, texcoord);
    frag_color.x += r;
    frag_color.y += g;
    frag_color.z += b;
    frag_color.a = transparency;
}