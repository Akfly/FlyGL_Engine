/* ---------------------------------------------------------------------------
** DizzyProcess.hpp
** Doubles the screen and mixes it in a radius.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#ifndef DIZZYPROCESS_HEADER
#define DIZZYPROCESS_HEADER

#include "Postprocess.hpp"

    namespace flygl
    {
        class DizzyProcess : public Postprocess
        {
        protected:
            GLuint deltaTimeID;
            GLuint radiusID;
            GLuint viewHeightID;

            GLfloat deltaTime;
            GLfloat radius;
            GLuint viewHeight;

        public:

			//Constructor
			DizzyProcess(): Postprocess()
			{
			}

            ~DizzyProcess()
            {
                Postprocess::~Postprocess();
            }

            void Update(const float& delta_time)
            {
                deltaTime += delta_time;
            }

            virtual void Initialize(
                const std::string& vertex_path, const std::string& fragment_path, 
                const int& screenWidth,         const int& screenHeight)
            {
                Postprocess::Initialize(vertex_path, fragment_path, screenWidth, screenHeight);
                
                deltaTime = 0.0f;
                radius    = 10.0f;
                viewHeight = screenHeight;

                deltaTimeID  = postProcessShader.SetUniform("deltaTime");
                radiusID     = postProcessShader.SetUniform("radius");
                viewHeightID = postProcessShader.SetUniform("viewHeight");
            }

            // Get the new height value
            virtual void Resize(const int& screenWidth, const int& screenHeight)
            {
                Postprocess::Resize(screenWidth, screenHeight);
                viewHeight = screenHeight;
            }

        protected:

            // Render the Uniforms
            virtual void RenderUniforms()
            {
                Postprocess::RenderUniforms();

                glUniform1f(deltaTimeID, deltaTime);
                glUniform1f(radiusID, radius);
                glUniform1i(viewHeightID, viewHeight);
            }
        };
    }

#endif
