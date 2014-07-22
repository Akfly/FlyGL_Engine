/* ---------------------------------------------------------------------------
** Postprocess.cpp
** A class that represent the post-processing effects (After drawing effects).
** It can be used for simple post process or as a parent class for a more
** complicated ones.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#include <GL\glew.h>
#include "Postprocess.hpp"

namespace flygl
{
    // Initialize the necessary data to start the Postprocessing
    void Postprocess::Initialize(
        const std::string& vertex_path, const std::string& fragment_path,
        const int& screenWidth, const int& screenHeight)
    {
        attributeCounter = 0;

        //Screen quad
        InitializeRenderQuad();
        InitializeShaders   (vertex_path, fragment_path);
        InitializeAttributes();

        //Create Frame buffer
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        
        InitializeTexturesSection(screenWidth, screenHeight);
        CreateRenderBuffer       (screenWidth, screenHeight);
    }

    // Applies the Postprocessing
    void Postprocess::Draw()
    {
        // Bind the final render buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        postProcessShader.UseThisShader();
        
        RenderTextures();
        RenderUniforms();
        
        // Enable attributes
        for(GLuint i = 0; i < attributeCounter; ++i)
        {
            glEnableVertexAttribArray(i);
        }
        
        DrawAttributes();

        // Draw the Quad that will be on the screen
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
 
        // Disable attributes
        for(GLuint i = 0; i < attributeCounter; ++i)
        {
            glDisableVertexAttribArray(i);
        }
    }

    // Initializes the Quad that will be on the screen and that will have
    // the texture for postprocessing
    void Postprocess::InitializeRenderQuad()
    {
        glGenVertexArrays(1, &vaoQuad);
        glBindVertexArray(vaoQuad);
        
        // An array of 3 vectors which represents 3 vertices
        static const GLfloat g_vertex_buffer_data[] = 
        {
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f,
             1.0f, 1.0f, 0.0f,
             1.0f,-1.0f, 0.0f,
        };
        // Generate 1 buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(1, &vboQuad);
        glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    }

    // Initializes a texture with the given parameters
    //
    // texture_id   The uniform ID
    // texture      The texture itself
    // attachment   GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, etc...
    // uniform_name The name that the uniform has on the shader
    // screenWidth  The width size of the viewport
    // screenHeight The height size of the viewport
    void Postprocess::InitializeTexture(
        GLuint& texture_id, GLuint& texture, 
        const GLenum& attachment, const std::string& uniform_name, 
        const int& screenWidth, const int& screenHeight)
    {
        texture_id = postProcessShader.SetUniform(uniform_name);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D
        (
            GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D
        (
            GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0
        );
        drawingBuffers.push_back(attachment);
    }
}