/* ---------------------------------------------------------------------------
** motionBlurVertex
** ---------------------------------------------------------------------------
** A shader that blurs the movement. This motion blur only affects the camera.
** The Vertex just passes the coordinates to the Fragment.
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