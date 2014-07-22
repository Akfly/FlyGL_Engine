/* ---------------------------------------------------------------------------
** ShaderManager.cpp
** Class that manages the loading, compiling and variable managing of the
** shaders.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#include "ShaderManager.hpp"

#include <string>            //String
#include <cassert>           //Assert

//File management
#include <iostream>
#include <fstream>

//GL
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

namespace flygl
{

    // Loads the vertex shader
    // Parameters:
    // path     The Path route of the vertex shader file
	void ShaderManager::LoadVertexShader(std::string path)
	{
		vertex_shader_code = "";

		std::ifstream infile;
		infile.open (path, std::ifstream::in);

		while (infile.good())
		{
            char character = (char) infile.get();
            if(infile.good())
            {
			    vertex_shader_code += character;
            }
		}
	}

    // Loads the fragment shader
    // Parameters:
    // path     The Path route of the fragment shader file
	void ShaderManager::LoadFragmentShader(std::string path)
	{
		fragment_shader_code = "";

		std::ifstream infile;
		infile.open (path, std::ifstream::in);

		while (infile.good())
		{
            char character = (char) infile.get();
            if(infile.good())
            {
			    fragment_shader_code += character;
            }
		}
	}

    //Compiles the previously loaded shaders, checking for errors
	void ShaderManager::CompileShaders ()
	{
		GLint succeeded = GL_FALSE;

		// Creates shader objects

		GLuint   vertex_shader_id = glCreateShader (GL_VERTEX_SHADER  );
		GLuint fragment_shader_id = glCreateShader (GL_FRAGMENT_SHADER);

		// Load shaders code

		const char *   vertex_shaders_code[] = {   vertex_shader_code.c_str () };
		const char * fragment_shaders_code[] = { fragment_shader_code.c_str () };
		const GLint    vertex_shaders_size[] = {   vertex_shader_code.size  () };
		const GLint  fragment_shaders_size[] = { fragment_shader_code.size  () };

		glShaderSource  (  vertex_shader_id, 1,   vertex_shaders_code,   vertex_shaders_size);
		glShaderSource  (fragment_shader_id, 1, fragment_shaders_code, fragment_shaders_size);

		// Compile shaders

		glCompileShader (  vertex_shader_id);
		glCompileShader (fragment_shader_id);

		// Check if the compilation was a success

		glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &succeeded);
		if (!succeeded)
        {
            ShowCompilationError (  vertex_shader_id);
        }

		glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &succeeded);
		if (!succeeded) 
        {
            ShowCompilationError (fragment_shader_id);
        }

		// Create the program ID

		program_id = glCreateProgram ();

		// Load the shaders into the program

		glAttachShader  (program_id,   vertex_shader_id);
		glAttachShader  (program_id, fragment_shader_id);

		// Link the shaders

		glLinkProgram   (program_id);

		// Check if the link was successful

		glGetProgramiv  (program_id, GL_LINK_STATUS, &succeeded);
		if (!succeeded)
        {
            ShowLinkageError (program_id);
        }

		// Free the shaders

		glDeleteShader (  vertex_shader_id);
		glDeleteShader (fragment_shader_id);
	}

    //Shows an error log
    void ShaderManager::ShowCompilationError (GLuint shader_id)
	{
		std::string info_log;
		GLint  info_log_length;

		glGetShaderiv (shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

		info_log.resize (info_log_length);

		glGetShaderInfoLog (shader_id, info_log_length, NULL, &info_log.front ());

		std::cerr << info_log.c_str () << std::endl;

		#ifdef _MSC_VER
			OutputDebugStringA (info_log.c_str ());
		#endif

		assert(false);
	}

    //Shows an error log
	void ShaderManager::ShowLinkageError (GLuint program_id)
	{
		std::string info_log;
		GLint  info_log_length;

		glGetProgramiv (program_id, GL_INFO_LOG_LENGTH, &info_log_length);

		info_log.resize (info_log_length);

		glGetProgramInfoLog (program_id, info_log_length, NULL, &info_log.front ());

		std::cerr << info_log.c_str () << std::endl;

		#ifdef _MSC_VER
			OutputDebugStringA (info_log.c_str ());
		#endif

		assert(false);
	}
}
