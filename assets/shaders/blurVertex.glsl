/* ---------------------------------------------------------------------------
** blurVertex
** ---------------------------------------------------------------------------
** A shader that blurs the screen. The vertex just pass the coordinates 
** to the fragment.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#version 330

in vec2 position;
out vec2 Texcoord;

void main() 
{
    Texcoord = (position / 2.0) + 0.5;
    gl_Position = vec4(position, 0.0, 1.0);
}