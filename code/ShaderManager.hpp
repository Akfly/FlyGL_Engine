/* ---------------------------------------------------------------------------
** ShaderManager.hpp
** Class that manages the loading, compiling and variable managing of the
** shaders.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

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

    class ShaderManager
    {
    private:

		GLuint program_id;

        // The shaders
		std::string vertex_shader_code;
		std::string fragment_shader_code;

    public:

        // Constructor
		ShaderManager(const std::string vertex_shader, const std::string fragment_shader):
		  vertex_shader_code(vertex_shader), fragment_shader_code(fragment_shader)
		{}

        // Constructor
		ShaderManager()
		{}

        // Destructor
		~ShaderManager()
		{
			glDeleteProgram (program_id);
		}

    	void LoadVertexShader  (std::string path);
		void LoadFragmentShader(std::string path);
        void CompileShaders();

        //Use this program (after loading and compiling)
        inline void UseThisShader()
        {
            glUseProgram (program_id);
        }

        // Sets a uniform with the given name.
        //Returns the ID of the Uniform.
        GLuint SetUniform(const std::string &u_name) const
        {
            return glGetUniformLocation(program_id, u_name.c_str());
        }

        // Returns the Shader Program
        GLuint GetProgram() const
        {
            return program_id;
        }

    private:

        void ShowCompilationError (GLuint shader_id );
		void ShowLinkageError     (GLuint program_id);
    };

}
#endif