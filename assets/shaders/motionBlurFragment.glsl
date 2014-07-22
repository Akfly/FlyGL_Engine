/* ---------------------------------------------------------------------------
** motionBlurVertex
** ---------------------------------------------------------------------------
** A shader that blurs the movement. This motion blur only affects the camera.
** The Fragment takes the speedTexture, and depending on that value sets an
** offset. The pixel will be blurred depending on that offset.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#version 330

in vec2 Texcoord;
out vec4 outColor;

uniform sampler2D speedTexture;
uniform sampler2D colorTexture;

uniform float intensity;
uniform int numberOfSamples;

void main() 
{
	//If the pixel in speedTexture is pure black, probably it is not a mesh, and anyway it is not moving. So paint it black
	bool drawPixel = (texture2D(speedTexture, Texcoord).rgb != vec3(0.0, 0.0, 0.0));
	
	if(drawPixel)
	{
		//float intensity = 1.5;
		
		vec2 speed  = 2.0 * texture2D(speedTexture, Texcoord).rg - 1.0;
		vec2 blur_direction = intensity * speed;
		vec3 color  = vec3(0.0, 0.0, 0.0);
		
		//int numberOfSamples = 8;
		
		for (float i = 0.0; i < numberOfSamples; i++)
		{
			vec2 offset = blur_direction * (float(i) / float(numberOfSamples - 1) - 0.5);
			color += texture2D(colorTexture, Texcoord + offset).rgb;
		}
		
		color /= numberOfSamples;
		outColor = vec4(color, 1.0);
	}
	else
	{
		outColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}