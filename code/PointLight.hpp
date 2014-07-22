/* ---------------------------------------------------------------------------
** PointLight.hpp
** Represents a light in the world. Only PointLights, sorry, no other light
** implementations. You can still change its color, intensity, etc...
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#ifndef POINT_HEADER
#define POINT_HEADER

#include "Actor.hpp"
#include <vector>

    namespace flygl
    {
        // This is a struct to handle multiple lights on the scene.
        // Used in the View class, but since it is really Light related
        // I create it here xD
        struct LightingBuffer
        {
        public:

            static const short MAX_LIGHT_NUMBER = 8;

            GLsizei numberOfLights;
            
            std::vector<GLfloat> positionBuffer;
            std::vector<GLfloat> colorBuffer;
            std::vector<GLfloat> intensityBuffer;
        };

        class PointLight: public Actor
        {
        protected:
            
            glm::vec3 color;
            float     intensity;
            float     prevIntensity;
            bool      isEnabled;


        public:

			// Constructor
			PointLight():Actor(), color(0.0f, 0.0f, 0.0f)
            {
                intensity = 60.0f;
                isEnabled = true;
                prevIntensity = intensity;
            }

			// Sets the transformation buffer
			virtual void Update()
			{
                Actor::Update();
			}

            // Changes the color of the light 
            void SetColor(const float& r, const float& g, const float& b)
            {
                color.x = r;
                color.y = g;
                color.z = b;
            }

            // Changes the color of the light
            void SetColor(const glm::vec3& _color)
            {
                color = _color;
            }

            // Returns the color of the light
            glm::vec3 GetColor() const
            {
                return color;
            }

            // Changes the intensity of the light
            void SetIntensity(const float& w)
            {
                prevIntensity = w;

                if(isEnabled)
                {
                    intensity = w;
                }
            }

            // Returns the intensity of the light
            float GetIntensity() const
            {
                return intensity;
            }

            // Switches the light on
            void TurnOn()
            {
                intensity = prevIntensity;
                isEnabled = true;
            }

            // Switches the light off
            void TurnOff()
            {
                intensity = 0.0f;
                isEnabled = false;
            }

            // If on, turns it on, and viceversa
            void Switch()
            {
                if(isEnabled)
                {
                    TurnOff();
                }
                else
                {
                    TurnOn();
                }
            }

            // If the light is on or off
            const bool& IsLightEnable() const
            {
                return isEnabled;
            }
        };
    }

#endif
