/* ---------------------------------------------------------------------------
** Postprocess.hpp
** A class that represent the post-processing effects (After drawing effects).
** It can be used for simple post process or as a parent class for a more
** complicated ones.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#ifndef POSTPROCESS_HEADER
#define POSTPROCESS_HEADER

#include "ShaderManager.hpp"
#include <GL\glew.h>
#include <string>
#include <vector>

    namespace flygl
    {
        class Postprocess
        {
        protected:
            //Scene Buffers
            GLuint frameBuffer;
            GLuint renderDepthStencilBuffer;

            //The color texture
            GLuint textureColor;
            GLuint postProcessTextureID;

            // The mesh that will serve as the new screen!!
            GLuint vaoQuad;
            GLuint vboQuad;
            
            // The attribute Position
            GLuint attPosition;
            
            // The shaders
            ShaderManager postProcessShader;

            // An array of drawing buffers, if we have more than one texture target
            std::vector<GLenum> drawingBuffers;

            GLuint attributeCounter;

        public:

			// Constructor
			Postprocess()
			{
			}

            // Destructor
            ~Postprocess()
            {
                glDeleteFramebuffers (1, &frameBuffer             );
                glDeleteRenderbuffers(1, &renderDepthStencilBuffer);
                glDeleteTextures     (1, &textureColor            );
            }

            virtual void Initialize(
                const std::string& vertex_path, const std::string& fragment_path, 
                const int& screenWidth,         const int& screenHeight);

            virtual void Draw();

            // Call this before drawing anything, so we switch the frame Buffer
            void PreProcess()
            {
                glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
                glDrawBuffers(2, &drawingBuffers[0]);
                glEnable(GL_DEPTH_TEST);
            }

            // If the screen changes its size, we must change the size of the texture and the buffer
            virtual void Resize(const int& screenWidth, const int& screenHeight)
            {
                glBindTexture(GL_TEXTURE_2D, textureColor);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
            }

        protected:

            // Initialize the attributes that will be passed to the shader
            virtual void InitializeAttributes()
            {
                attPosition = glGetAttribLocation(postProcessShader.GetProgram(), "position");
                glEnableVertexAttribArray(attPosition);
                glVertexAttribPointer(attPosition, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
                attributeCounter++;
            }

            // Called to initialize every texture
            virtual void InitializeTexturesSection(const int& screenWidth, const int& screenHeight)
            {
                InitializeTexture(postProcessTextureID, textureColor, 
                    GL_COLOR_ATTACHMENT0, "colorTexture", screenWidth, screenHeight);
            }

            // If Uniforms are required, render them here
            virtual void RenderUniforms()
            {}

            // Render the texture. If more textures are needed to render, override this method
            virtual void RenderTextures()
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureColor);
                glUniform1i    (postProcessTextureID, 0);
            }

            // Pass the attributes to the shader. If more attributes are needed, then override this method
            virtual void DrawAttributes()
            {
                glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            }

            void InitializeTexture(
                GLuint& texture_id,       GLuint& texture, 
                const GLenum& attachment, const std::string& uniform_name, 
                const int& screenWidth,   const int& screenHeight);

        private:

            void InitializeRenderQuad();

            // Create Renderbuffer Object to hold depth and stencil buffers
            void CreateRenderBuffer(const int& screenWidth, const int& screenHeight)
            {
                glGenRenderbuffers       (1, &renderDepthStencilBuffer);
                glBindRenderbuffer       (GL_RENDERBUFFER, renderDepthStencilBuffer);
                glRenderbufferStorage    (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER,  GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderDepthStencilBuffer);
            }

            // Initialize the shaders that have the Postprocess effect
            void InitializeShaders(const std::string& vertex_path, const std::string& fragment_path)
            {
                postProcessShader.LoadVertexShader  ( vertex_path   );
                postProcessShader.LoadFragmentShader( fragment_path );
                postProcessShader.CompileShaders();
                postProcessShader.UseThisShader();
            }
        };
    }

#endif
