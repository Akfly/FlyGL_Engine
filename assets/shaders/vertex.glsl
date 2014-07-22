/* ---------------------------------------------------------------------------
** Vertex
** ---------------------------------------------------------------------------
** A shader that draws a mesh with diffuse map, normal map, and specular map.
** It has some lights and outputs data for a postprocess.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#version 330 core

//Attributes
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor_modelspace;
layout(location = 2) in vec2 vertexUV_modelspace;
layout(location = 3) in vec3 vertexNormal_modelspace;
layout(location = 4) in vec3 tangents;
layout(location = 5) in vec3 bitangents;


//Uniforms
uniform mat4 MVP;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 modelView3x3;
uniform vec3 lightPos[8];
uniform int numberOfLights;
uniform mat4 oldMVP;	//For motion blur

//Varying
out vec3 fragColor;
out vec2 uv;
out vec3 fragPosition;
out vec3 fragNormal;
out vec3 lightDirectionTan[8];
out vec3 eyeDirectionTan;
out vec4 oldScreenCoord;	// For motion blur
out vec4 newScreenCoord;

void main()
{
	vec4 pos          = vec4(vertexPosition_modelspace, 1.0);
    gl_Position       = MVP * pos;
	fragColor         = vertexColor_modelspace;
	uv                = vertexUV_modelspace;
	fragPosition      = (modelMatrix * pos).xyz;
	fragNormal        = (viewMatrix * modelMatrix * vec4(vertexNormal_modelspace,0.0)).xyz;
	
	// For normal mapping
	mat3 TBN = transpose(mat3
	(
		modelView3x3 * normalize(tangents  ),
		modelView3x3 * normalize(bitangents),
		modelView3x3 * normalize(vertexNormal_modelspace   )
	));
	
	vec3 vertexPosCam = (viewMatrix * modelMatrix * pos).xyz;
	vec3 eyeDirection = vec3(0,0,0) - vertexPosCam;
	eyeDirectionTan   = TBN * eyeDirection;
	for(int i = 0; i < numberOfLights; i++)
	{
		vec3 lightPosCam     = (viewMatrix * vec4(lightPos[i], 1.0)).xyz;
		lightDirectionTan[i] = TBN * lightPosCam  + eyeDirection;
	}
	
	// For Motion Blur
	oldScreenCoord = oldMVP * pos;
    newScreenCoord = MVP * pos;
}
