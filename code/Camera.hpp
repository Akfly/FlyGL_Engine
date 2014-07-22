/* ---------------------------------------------------------------------------
** Camera.hpp
** This class represents a Camera that will give a point of view to the scene.
** It has different values, like near and far planes or Field of View.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#include "Actor.hpp"

// GLM
#include <glm/glm.hpp>

    namespace flygl
    {
        class Camera: public Actor
        {
        protected:
         
            float fieldOfView;
            float nearPlane;
            float farPlane;

            int screenWidth;
            int screenHeight;

        public:

			//Constructor
			Camera():Actor()
            { 
                fieldOfView =  45.0f; 
                nearPlane   =   0.1f;
                farPlane    = 1000.0f;
            }
            
			//Sets the transformation buffer
			virtual void Update()
			{
                Actor::Update();
			}

            // Changes the actual Field of View. Smaller means 
            // zoom in, higher zoom out
            void SetFoV(const float& fov)
            {
                if(fov > 0.0f && fov < 180.0f)
                {
                    fieldOfView = fov;
                }
            }

            // Changes the size of the screen, necessary for calculating
            // the Projection Matrix if the screen has been resized.
            void SetScreenSize(const int& width, const int& height)
            {
                screenWidth = width;
                screenHeight = height;
            }

            // Calculates the Projection Matrix with the actual Camera Data.
            glm::mat4x4 GetProjectionMatrix() const
            {
                return glm::perspective (fieldOfView, GLfloat(screenWidth) / screenHeight, nearPlane, farPlane);
            }

            // Returns the actual value of the Near Plane
            float GetNear() const
            {
                return nearPlane;
            }

            // Returns the actual value of the Far Plane
            float GetFar() const
            {
                return farPlane;
            }
        };
    }

#endif
