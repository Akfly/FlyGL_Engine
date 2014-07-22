/* ---------------------------------------------------------------------------
** View.hpp
** Class that manages all the visualization of the scene.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#ifndef VIEW_HEADER
#define VIEW_HEADER

    // Glew
    #include <GL/glew.h>
    
    // GLM
    #include <glm/glm.hpp>

    #include "Mesh.hpp"
    #include "Camera.hpp"
    #include "PointLight.hpp"
    #include "Postprocess.hpp"
    #include "MotionBlur.hpp"
    #include "DizzyProcess.hpp"
    
    namespace flygl
    {

        class View
        {
            enum Effects
            {
                NONE        = 0,
                REFLECTION  = 1,
                MOTION_BLUR = 2,
                BLUR        = 3,
                DIZZY       = 4
            }
            actualEffect;

        private:

            // Scene Meshes
            Mesh   bat;
            Mesh   floor;
            Mesh   walls;
            Mesh   columns;

            // Lights
            LightingBuffer lightBuffer;
            PointLight     whiteLight;
            PointLight     redLight;

            float totalTime;

            ////CAMERA DATA
            Camera cam;
            float camMovementSpeed;
            float camRotationSpeed;

            //Viewport
            int screenWidth;
            int screenHeight;

            //POST-PROCESSING IS SO COOL!!!
            MotionBlur  blur;
            Postprocess normal_blur;
            DizzyProcess dizzy;

        public:

            View(const int& width, const int& height);

            void   Update (const float& deltaTime);
            void   Draw   ();
            void   Resize (const int& width, const int& height);

            void   ShowLoading();

        private:

            void Inputs(const float& deltaTime);
            void CalculateLightingBuffer();

            void NormalDraw    (const glm::mat4& projection_matrix, const glm::mat4& view_matrix);
            void ReflectionDraw(const glm::mat4& projection_matrix, const glm::mat4& view_matrix);
            void PrepareReflectiveDraws();
            void PrepareReflectionDraws();
            void EndReflection();

            void DrawMeshReflection(Mesh& mesh, const glm::mat4& projection_matrix, const glm::mat4& view_matrix);

            void PostProcessInitialization();
            void CameraInitialization();
            void LightsInitialization();
            void MeshInitialization();

        };

    }

#endif
