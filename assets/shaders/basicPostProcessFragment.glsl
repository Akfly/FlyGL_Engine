/* ---------------------------------------------------------------------------
** basicPostProcessFragment
** ---------------------------------------------------------------------------
** A simple post process shader that does nothing. The Fragment just draws
** the color Texture into the output.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#version 330

in vec2 Texcoord;
out vec4 outColor;

uniform sampler2D colorTexture;

void main() 
{
    outColor = texture(colorTexture, Texcoord);
}