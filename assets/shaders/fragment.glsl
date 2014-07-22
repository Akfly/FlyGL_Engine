/* ---------------------------------------------------------------------------
** Fragment
** ---------------------------------------------------------------------------
** A shader that draws a mesh with diffuse map, normal map, and specular map.
** It has some lights and outputs data for a postprocess.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#version 330 core

out vec4 color;
out vec4 speedTexture;


//Varying
in vec3 fragColor;
in vec2 uv;
in vec3 fragPosition;
in vec3 fragNormal;
in vec3 eyeDirectionTan;
in vec3 lightDirectionTan[8];
in vec4 oldScreenCoord;
in vec4 newScreenCoord;

uniform sampler2D diffuseSampler;
uniform sampler2D specularSampler;
uniform sampler2D normalSampler;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec3 lightPos[8];
uniform vec3 lightColor[8];
uniform float lightPower[8];
uniform int numberOfLights;


void main()
{
	const int MAX_NUMBER_OF_LIGHTS = 8;
	int actualLightNumber = numberOfLights;
	
	if(actualLightNumber > MAX_NUMBER_OF_LIGHTS)
	{
		actualLightNumber = MAX_NUMBER_OF_LIGHTS;
	}
		
	//Material Colors
	vec3 materialDiffuseColor  = texture2D(diffuseSampler, uv).rgb;
	vec3 materialAmbientColor  = vec3(0.1,0.1,0.1) * materialDiffuseColor;
	vec3 materialSpecularColor = vec3(0.3,0.3,0.3);
	
	vec3 normal = normalize(texture2D(normalSampler, uv).rgb * 2.0 - 1.0);
	
	//Specular Init
	float shininess        = texture2D(specularSampler, uv).r * 255.0;
	vec3  normalizedEyeDir = normalize(eyeDirectionTan);
	
	vec3 totalLighting = vec3(materialAmbientColor);
	
    for (int i = 0; i < actualLightNumber; i++) // for all light sources
	{
		// Lights
		float distanceToLight = length   (lightPos[i] - fragPosition);	
		float distancePowered = distanceToLight * distanceToLight;
		vec3  light_direction = normalize(lightDirectionTan[i]      );
		
		//Diffuse
		float diffPower       = clamp(dot(normal, light_direction), 0, 1);
		
		//Specular
		vec3  reflectDirection = reflect  (-light_direction, normal);
		float basicSpecPower   = max    (dot(normalizedEyeDir, reflectDirection), 0.0);
		float specPower        = pow      (basicSpecPower, shininess);
		
		// Apply Lighting
		vec3 specTotal = materialSpecularColor * lightColor[i] * (lightPower[i] / 100.0) * specPower / distancePowered;	//Specular
		vec3 diffTotal = materialDiffuseColor  * lightColor[i] *  lightPower[i]          * diffPower / distancePowered;	//Diffuse
		
		totalLighting += diffTotal + specTotal;
	}
	
    color = vec4(totalLighting, 0.3);
	
	// Motion Blur
	vec2 speed = (newScreenCoord.xy / newScreenCoord.w) - (oldScreenCoord.xy / oldScreenCoord.w);
	speedTexture = vec4(0.5 + 0.5 *  speed, 0.0, 1.0);
}
