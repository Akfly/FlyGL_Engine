/* ---------------------------------------------------------------------------
** main.cpp
** This project is a simple OpenGL engine. It loads some objs, and applies
** some cool effects :).
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/


// STB IMAGE, for image loading
#define STB_IMAGE_IMPLEMENTATION 

// assert
#include <cassert>

// glew
#include <GL/glew.h>

// SFML
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "View.hpp"
#include "ShaderManager.hpp"

using namespace sf;

void EventHandler(Window &window, flygl::View &view, bool &running);
void ShowLoading();

int main ()
{
    static const sf::Uint32 INITIAL_WINDOW_WIDTH  = 640;
    static const sf::Uint32 INITIAL_WINDOW_HEIGHT = 400;

    // Create SFML window
    Window window(VideoMode(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT), "FlyEngine", Style::Default, ContextSettings(32));
    
    // Glew initialization
	glewExperimental           = true;
    GLenum glew_initialization = glewInit ();
    assert(glew_initialization == GLEW_OK);

    window.setVerticalSyncEnabled (true);

    // Show the loading screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ShowLoading();
    window.display ();

    flygl::View view(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);

    bool running = true;

    // Delta Time Initialization
    sf::Clock deltaClock;
    sf::Time deltaTime = deltaClock.restart();

    // Show Loading Scren
    
    //view.ShowLoading();
    

    // Core Loop
    do
    {
        EventHandler(window, view, running);
        
        // Updates
        deltaTime = deltaClock.restart();
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        view.Update(deltaTime.asSeconds());
        
        // Draw
        view.Draw();

        // Pass everything to the Window.
        window.display ();
    }
    while (running);

    // Bye-bye
    return (EXIT_SUCCESS);
}

// Takes care of all window events
// 
// window   The window, to know the inputs
// view     If the window is resized, we must inform the view with the new parameters
// running  A boolean to tell if the application is being closed
void EventHandler(Window &window, flygl::View &view, bool &running)
{
    Event event;

    while (window.pollEvent (event))
    {
        switch (event.type)
        {
            // Close
            case Event::Closed:
            {
                running = false;
                break;
            }

            // Resize
            case Event::Resized:
            {
                Vector2u window_size = window.getSize ();

                view.Resize (window_size.x, window_size.y);

                break;
            }
        }
    }
}

// Show a nice loading screen at the begining
void ShowLoading()
{
    //Initialize the quad.

    GLuint vaoQuad;
    GLuint vboQuad;

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

    flygl::ShaderManager loadingshader;

    loadingshader.LoadVertexShader  ("../../assets/shaders/loadingVertex.glsl");
    loadingshader.LoadFragmentShader("../../assets/shaders/loadingFragment.glsl");
    loadingshader.CompileShaders();
    loadingshader.UseThisShader();

    GLuint attPosition = glGetAttribLocation(loadingshader.GetProgram(), "position");
    glEnableVertexAttribArray(attPosition);
    glVertexAttribPointer(attPosition, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    int img_width, img_height, comp_num;
    unsigned char *data = stbi_load("../../assets/textures/loading.png", &img_width, &img_height, &comp_num, 0);

    // Create one OpenGL texture
    GLuint texture_id = loadingshader.SetUniform("colorTexture");

	GLuint loadingtextureID;
	glGenTextures(1, &loadingtextureID);
	
	glBindTexture(GL_TEXTURE_2D, loadingtextureID);

    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 
                    img_width, img_height, 
                    0, GL_RGB, GL_UNSIGNED_BYTE, 
                    data);

    // Free the data, we already have it stored
    stbi_image_free(data);

    // Set texture Parameters
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST      );
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST      );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, loadingtextureID);
    glUniform1i    (texture_id, 0);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

    glDisableVertexAttribArray(0);

    glDeleteTextures(1, &loadingtextureID);
    glDeleteBuffers(1, &vboQuad   );
}