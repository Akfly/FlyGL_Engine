/* ---------------------------------------------------------------------------
** View.hpp
** Class that manages all the visualization of the scene.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#include "View.hpp"

#include <SFML/Window.hpp>  //For SFML inputs

// STB Image, for image loading
#include "stb_image\stb_image.h" 

namespace flygl
{

    using namespace std;

    // Class Constructor, Initializes the values.
    View::View(const int& width, const int& height)
    {
        screenWidth  = width;
        screenHeight = height;
        totalTime = 0.0f;
        
        glEnable   (GL_DEPTH_TEST);
        glEnable   (GL_CULL_FACE ); // Backface Culling
        glDepthFunc(GL_LESS      ); // Accept closer fragments

        glClearColor (0.f, 0.f, 0.f, 1.f);

        MeshInitialization  ();
        CameraInitialization();
        LightsInitialization();

        Resize (screenWidth, screenHeight);

        PostProcessInitialization();
    }

    // Called every frame, updates the data
    void View::Update (const float& deltaTime)
    {
        totalTime += deltaTime;
        Inputs(deltaTime);

        if(actualEffect == DIZZY)
        {
            dizzy.Update(deltaTime);
        }

        whiteLight.SetPosition(-cam.GetPosition());

        redLight.SetIntensity((1 + glm::sin(totalTime)) * 100000.0f);

        bat.       Update();
        floor.     Update();
        walls.     Update();
        columns.   Update();
        cam.       Update();
        whiteLight.Update();
        redLight.  Update();
    }

    // Called every frame, draws on the screen
    void View::Draw ()
    {
        //Post-Processing: Preprocess
        if(actualEffect == MOTION_BLUR)
        {
            blur.PreProcess();
        }
        else if(actualEffect == BLUR)
        {
            normal_blur.PreProcess();
        }
        else if(actualEffect == DIZZY)
        {
            dizzy.PreProcess();
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glEnable(GL_DEPTH_TEST);
        }


        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4  ProjectionMatrix = cam.GetProjectionMatrix();
        glm::mat4& viewMatrix       = cam.GetMatrix();

        CalculateLightingBuffer();
        
        if(actualEffect == REFLECTION)
        {
            ReflectionDraw(ProjectionMatrix, viewMatrix);
        }
        else
        {
            NormalDraw(ProjectionMatrix, viewMatrix);
        }
        
        //Post-Process
        if(actualEffect == MOTION_BLUR)
        {
            blur.Draw();
        }
        else if(actualEffect == BLUR)
        {
            normal_blur.Draw();
        }
        else if(actualEffect == DIZZY)
        {
            dizzy.Draw();
        }
    }

    void View::NormalDraw(const glm::mat4& projection_matrix, const glm::mat4& view_matrix)
    {
        bat.    Draw(projection_matrix, view_matrix, lightBuffer);
        floor.  Draw(projection_matrix, view_matrix, lightBuffer);
        walls.  Draw(projection_matrix, view_matrix, lightBuffer);
        columns.Draw(projection_matrix, view_matrix, lightBuffer);
    }

    void View::ReflectionDraw(const glm::mat4& projection_matrix, const glm::mat4& view_matrix)
    {
        //Normal Draws
        bat.    Draw(projection_matrix, view_matrix, lightBuffer);
        walls.  Draw(projection_matrix, view_matrix, lightBuffer);
        columns.Draw(projection_matrix, view_matrix, lightBuffer);

        //Draws elements that will have reflexion
        PrepareReflectiveDraws();
        floor.Draw(projection_matrix, view_matrix, lightBuffer);

        //Redraw the elements that will be reflected
        PrepareReflectionDraws();
        DrawMeshReflection(bat,     projection_matrix, view_matrix);
        EndReflection();
    }

    // Resizes the screen with the given values
    void View::Resize (const int& width, const int& height)
    {
        screenWidth  = width;
        screenHeight = height;
        cam.SetScreenSize (screenWidth, screenHeight);

        glViewport (0, 0, width, height);
        blur.       Resize(width, height);
        normal_blur.Resize(width, height);
        dizzy.      Resize(width, height);
    }

    

    // Next draws will reflect meshes
    void View::PrepareReflectiveDraws()
    {
        glEnable(GL_STENCIL_TEST);

        //Stencil Buffer Operations
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0xFF); // Write to stencil buffer
        glDepthMask(GL_FALSE); // Don't write to depth buffer
        glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)
    }

    // Next draws will be reflected
    void View::PrepareReflectionDraws()
    {
        glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
        glStencilMask(0x00); // Don't write anything to stencil buffer
        glDepthMask(GL_TRUE); // Write to depth buffer
        
        //Prepare Alpha
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Finish the reflection process
    void View::EndReflection()
    {
        glDisable(GL_BLEND);
        glDisable(GL_STENCIL_TEST);
    }

    // Draws a mesh as a reflection. This is an aditional draw call.
    void View::DrawMeshReflection(Mesh& mesh, const glm::mat4& projection_matrix, const glm::mat4& view_matrix)
    {
        mesh.SetScale(bat.GetScale().x, -bat.GetScale().y, bat.GetScale().z);
        mesh.Update();
        mesh.Draw(projection_matrix, view_matrix, lightBuffer);
        mesh.SetScale(bat.GetScale().x, -bat.GetScale().y, bat.GetScale().z);
    }

    // Calculates a buffer that contains all the lighting
    void View::CalculateLightingBuffer()
    {
        lightBuffer.positionBuffer[0] =  whiteLight.GetPosition().x;
        lightBuffer.positionBuffer[1] =  whiteLight.GetPosition().y;
        lightBuffer.positionBuffer[2] =  whiteLight.GetPosition().z;
        lightBuffer.positionBuffer[3] =    redLight.GetPosition().x;
        lightBuffer.positionBuffer[4] =    redLight.GetPosition().y;
        lightBuffer.positionBuffer[5] =    redLight.GetPosition().z;
   
        lightBuffer.colorBuffer[0] =  whiteLight.GetColor().x;
        lightBuffer.colorBuffer[1] =  whiteLight.GetColor().y;
        lightBuffer.colorBuffer[2] =  whiteLight.GetColor().z;
        lightBuffer.colorBuffer[3] =    redLight.GetColor().x;
        lightBuffer.colorBuffer[4] =    redLight.GetColor().y;
        lightBuffer.colorBuffer[5] =    redLight.GetColor().z;

        lightBuffer.intensityBuffer[0] =  whiteLight.GetIntensity();
        lightBuffer.intensityBuffer[1] =    redLight.GetIntensity();
    }

    // Handle the user inputs
    void View::Inputs(const float& deltaTime)
    {
        // CAMERA ROTATION
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
        {
            cam.Rotate(-camRotationSpeed * deltaTime, 0.0f, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
        {
            cam.Rotate(camRotationSpeed * deltaTime, 0.0f, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
        {
            cam.Rotate(0.0f, -camRotationSpeed * deltaTime, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
        {
            cam.Rotate(0.0f, camRotationSpeed * deltaTime, 0.0f);
        }

        // CAMERA MOVEMENT
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            cam.Move(0.0f, 0.0f, camMovementSpeed * deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            cam.Move(0.0f, 0.0f, -camMovementSpeed* deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            cam.Move(camMovementSpeed * deltaTime, 0.0f, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            cam.Move(-camMovementSpeed * deltaTime, 0.0f, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
        {
            cam.Move(0.0f, camMovementSpeed * deltaTime, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        {
            cam.Move(0.0f, -camMovementSpeed * deltaTime, 0.0f);
        }

        // MONSTER ROTATION
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            bat.Rotate(0.0f, -camRotationSpeed * deltaTime, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            bat.Rotate(0.0f, camRotationSpeed * deltaTime, 0.0f);
        }

        // LIGHT CONTROLS
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            whiteLight.SetIntensity(whiteLight.GetIntensity() + 100.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            whiteLight.SetIntensity(whiteLight.GetIntensity() - 100.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            whiteLight.Switch();
        }

        // SWITCH EFFECT
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0))
        {
            actualEffect = NONE;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
        {
            actualEffect = MOTION_BLUR;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
        {
            actualEffect = REFLECTION;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
        {
            actualEffect = BLUR;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
        {
            actualEffect = DIZZY;
        }
    }

    // Initialize Post-Processes data Here!
    void View::PostProcessInitialization()
    {
        actualEffect = NONE;

        blur.Initialize("../../assets/shaders/motionBlurVertex.glsl", 
                        "../../assets/shaders/motionBlurFragment.glsl",
                        screenWidth, screenHeight);
        
        normal_blur.Initialize("../../assets/shaders/blurVertex.glsl", 
                        "../../assets/shaders/blurFragment.glsl",
                        screenWidth, screenHeight);

        dizzy.Initialize("../../assets/shaders/dizzyVertex.glsl", 
                        "../../assets/shaders/dizzyFragment.glsl",
                        screenWidth, screenHeight);
    }
    
    // Initialize Camera data Here!
    void View::CameraInitialization()
    {
        cam.SetScreenSize (screenWidth, screenHeight);
        cam.SetPosition   ( -60.0f,  -160.0f, -230.0f);
        cam.SetRotation   ( 20.0f, -10.0f, 0.0f);
        cam.SetFoV        (45.0f);

        camMovementSpeed =  100.0f;
        camRotationSpeed = 200.0f;
    }

    // Initialize Light data Here!
    void View::LightsInitialization()
    {
        lightBuffer.numberOfLights = 3;

        lightBuffer.positionBuffer .resize(lightBuffer.numberOfLights * 3);
        lightBuffer.colorBuffer    .resize(lightBuffer.numberOfLights * 3);
        lightBuffer.intensityBuffer.resize(lightBuffer.numberOfLights    );

        whiteLight.SetPosition(300.0f, 200.0f, 50.0f);
        whiteLight.SetColor(1.0f, 1.0f, 1.0f);
        whiteLight.SetIntensity(10000.0f);

        redLight.SetPosition(-300.0f, 200.0f, 50.0f);
        redLight.SetColor(1.0f, 0.4f, 0.4f);
        redLight.SetIntensity(100000.0f);
    }

    // Initialize the mesh data here!
    void View::MeshInitialization()
    {
        bat.LoadMesh        ("../../assets/models/troll.obj");
        bat.LoadShaders     ("../../assets/shaders/vertex.glsl", "../../assets/shaders/fragment.glsl");
        bat.SetTexture      ("../../assets/textures/colors.jpg",   "diffuseSampler" );
        bat.SetTexture      ("../../assets/textures/specular.jpg", "specularSampler");
        bat.SetTexture      ("../../assets/textures/normals.jpg",  "normalSampler"  );
        bat.SetBasicUniforms();
        
        floor.LoadMesh        ("../../assets/models/suelo.obj");
        floor.LoadShaders     ("../../assets/shaders/vertex.glsl", "../../assets/shaders/fragment.glsl");
        floor.SetTexture      ("../../assets/textures/Suelo_D.tga",  "diffuseSampler");
        floor.SetTexture      ("../../assets/textures/Suelo_S.tga",  "specularSampler");
        floor.SetTexture      ("../../assets/textures/Suelo_NM.tga", "normalSampler");
        floor.SetBasicUniforms();

        walls.LoadMesh        ("../../assets/models/paredes.obj");
        walls.LoadShaders     ("../../assets/shaders/vertex.glsl", "../../assets/shaders/fragment.glsl");
        walls.SetTexture      ("../../assets/textures/Pared_D.tga",  "diffuseSampler");
        walls.SetTexture      ("../../assets/textures/Pared_S.tga",  "specularSampler");
        walls.SetTexture      ("../../assets/textures/Pared_NM.tga", "normalSampler");
        walls.SetBasicUniforms();

        columns.LoadMesh        ("../../assets/models/columnas.obj");
        columns.LoadShaders     ("../../assets/shaders/vertex.glsl", "../../assets/shaders/fragment.glsl");
        columns.SetTexture      ("../../assets/textures/Columna_D.tga",  "diffuseSampler");
        columns.SetTexture      ("../../assets/textures/Columna_S.tga",  "specularSampler");
        columns.SetTexture      ("../../assets/textures/Columna_NM.tga", "normalSampler");
        columns.SetBasicUniforms();
    }
}
