/* ---------------------------------------------------------------------------
** MotionBlur.hpp
** Adds the Motion Blur Post-Processing effect.
** This is a Motion Blur Camera Only (it is not independent for each object).
** The technique used is the Speed Buffer which is calculated on models
** shaders (actual MVP - previous MVP). This is passed to the Post processing
** shader as a texture, then we simply itearate a number of times to draw each
** pixel. The longer the distance (or higher the speed), more difference will
** be from the previous position to the actual one, so the iterations will draw
** wider, giving a blurry effect.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#ifndef MOTIONBLUR_HEADER
#define MOTIONBLUR_HEADER

#include "Postprocess.hpp"

    namespace flygl
    {
        class MotionBlur : public Postprocess
        {
        protected:
            GLuint speedTexture;
            GLuint speedTextureID;
            
            GLuint intensityID;
            GLuint samplesNumID;

            GLuint numberOfSamples;
            GLfloat intensity;

        public:

			//Constructor
			MotionBlur(): Postprocess()
			{
			}

            virtual void Initialize(
                const std::string& vertex_path, const std::string& fragment_path, 
                const int& screenWidth,         const int& screenHeight)
            {
                Postprocess::Initialize(vertex_path, fragment_path, screenWidth, screenHeight);
                
                numberOfSamples = 8;    //The higher the value, the cleaner (and more expensive)
                intensity = 0.7f;        //Raise for fun! :D

                intensityID = postProcessShader.SetUniform("intensity");
                samplesNumID = postProcessShader.SetUniform("numberOfSamples");
            }

            ~MotionBlur()
            {
                Postprocess::~Postprocess();
                glDeleteTextures(1, &speedTexture);
            }

            // Resize this new texture also!!
            virtual void Resize(const int& screenWidth, const int& screenHeight)
            {
                Postprocess::Resize(screenWidth, screenHeight);

                glBindTexture(GL_TEXTURE_2D, speedTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            }

        protected:
            
            virtual void InitializeTexturesSection(const int& screenWidth, const int& screenHeight)
            {
                Postprocess::InitializeTexturesSection(screenWidth, screenHeight);
                InitializeTexture(speedTextureID, speedTexture, 
                    GL_COLOR_ATTACHMENT1, "speedTexture", screenWidth, screenHeight);
            }

            // Render the Uniforms
            virtual void RenderUniforms()
            {
                Postprocess::RenderUniforms();

                glUniform1i(samplesNumID, numberOfSamples);
                glUniform1f(intensityID, intensity);
            }

            // Hey, we have one more texture, so we draw one more texture!!
            virtual void RenderTextures()
            {
                //Draws the color texture
                Postprocess::RenderTextures();

                //Here, the speed texture!
                glActiveTexture(GL_TEXTURE1);
                glBindTexture  (GL_TEXTURE_2D, speedTexture);
                glUniform1i    (speedTextureID, 1);
            }
        };
    }

#endif
