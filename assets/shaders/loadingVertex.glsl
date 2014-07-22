/* ---------------------------------------------------------------------------
** basicPostProcessVertex
** ---------------------------------------------------------------------------
** A simple post process shader that does nothing. The Vertex passes the
** position and coordinates to the Fragment.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#version 330

in vec2 position;
out vec2 Texcoord;

void main() 
{
    Texcoord = (position) + vec2(0.5, -0.5);
	Texcoord.y *= -1.0;
    gl_Position = vec4(position, 0.0, 1.0);
}