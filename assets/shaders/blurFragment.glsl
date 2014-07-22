#version 330

in vec2 Texcoord;
out vec4 outColor;

uniform sampler2D colorTexture;

const float blurSizeH = 1.0 / 300.0;
const float blurSizeV = 1.0 / 200.0;

void main() 
{
	vec4 sum = vec4(0.0);
	
    for (int x = -4; x <= 4; x++) //Two left, two right, and the center = 5
	{
        for (int y = -4; y <= 4; y++) // Same that x
		{
            sum += texture(colorTexture, vec2(Texcoord.x + x * blurSizeH, Texcoord.y + y * blurSizeV)) / 81.0; // 9*9 = 81
		}
	}
    outColor = sum;
}