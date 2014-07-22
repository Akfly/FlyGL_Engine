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
uniform float deltaTime;
uniform float radius;
uniform int viewHeight;

void main() 
{
	float invViewSize = 1.0 / viewHeight;
	
	vec3 color1 = texture(colorTexture, Texcoord + cos(deltaTime * 10.0) * radius * invViewSize).rgb;
	vec3 color2 = texture(colorTexture, Texcoord + sin(deltaTime * 10.0) * radius * invViewSize).rgb;
	
	
	
    outColor = vec4(mix(color1, color2, 0.5), 1.0);
	//outColor = vec4(color1, 1.0);
}