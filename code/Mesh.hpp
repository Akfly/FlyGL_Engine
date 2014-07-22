/* ---------------------------------------------------------------------------
** Mesh.hpp
** Class that represents a static mesh (loaded from an .obj file). It also
** manages some data to pass it to the shader.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#ifndef MESH_HEADER
#define MESH_HEADER

#include "Actor.hpp"

// glew
#include <GL/glew.h>       

// STB Image, for image loading
#include "stb_image\stb_image.h" 

#include "ShaderManager.hpp"
#include "PointLight.hpp"
#include "Camera.hpp"

    namespace flygl
    {
        class Mesh: public Actor
        {
        protected:
            
            static const GLuint ATTRIBUTE_COUNTER = 6;

            // Buffer data
            std::vector< glm::vec3 >    vertices;
            std::vector< glm::vec2 >    uvs;
            std::vector< glm::vec3 >    normals;
            std::vector<unsigned short> indices;
            std::vector<glm::vec3>      tangents;
            std::vector<glm::vec3>      bitangents;

            // GL Buffers
            GLuint     vertexBuffer;
            GLuint      colorBuffer;
            GLuint         uvBuffer;
            GLuint     normalBuffer;
            GLuint    tangentBuffer;
            GLuint  bitangentBuffer;
            GLuint    elementBuffer;

            glm::mat4 MVP;
            glm::mat4 oldMVP;   // Used in motion blur. The previous frame MVP

            ShaderManager shaders;

            // Textures
            std::vector<GLuint> textures;
            std::vector<GLuint> textureIDs;

            // Basic uniforms
            GLuint matrixID;
            GLuint viewMatrixID;
            GLuint modelMatrixID;
            GLuint modelView3x3ID;

            // Light uniforms
            GLuint lightPosID;
            GLuint lightColorID;
            GLuint lightPowerID;
            GLuint lightNumberID;

            // Cam Uniforms
            GLuint nearID;
            GLuint farID;

            // Motion Blur Uniform
            GLuint oldMVP_ID;

            
            
        public:

			//Constructor
			Mesh():Actor(), textures(0), textureIDs(0), oldMVP(0){}
            
            ~Mesh()
            {
                glDeleteTextures(textures.size(), &textures[0]);
                
                glDeleteBuffers(1, &vertexBuffer   );
                glDeleteBuffers(1, &colorBuffer    );
                glDeleteBuffers(1, &uvBuffer       );
                glDeleteBuffers(1, &normalBuffer   );
                glDeleteBuffers(1, &tangentBuffer  );
                glDeleteBuffers(1, &bitangentBuffer);
            }

			//Sets the transformation buffer
            virtual void Update()
			{
                Actor::Update();
			}

            void SetBasicUniforms();
            void LoadMesh        (const std::string& path);
            void LoadShaders     (const std::string& vertex_path,  const std::string& fragment_path);
            void SetTexture      (const std::string& texture_path, const std::string& uniform_name );
            void Draw            (const glm::mat4& projection_matrix, const glm::mat4& view_matrix, const LightingBuffer& lights);

        private:

            // Loading Methods

            void LoadMeshFromFile(const std::string path, std::vector<glm::vec3>& _vertices, std::vector<glm::vec2>& _uvs, std::vector<glm::vec3>& _normals);
            
            void ComputeTangents(
                const std::vector<glm::vec3>& _vertices, 
                const std::vector<glm::vec2>& _uvs, 
                const std::vector<glm::vec3>& _normals,
                      std::vector<glm::vec3>& _tangents, 
                      std::vector<glm::vec3>& _bitangents);

            void InitializeGLBuffers();

            // Drawing Methods

            void DrawUniforms(const glm::mat4& projection_matrix, const glm::mat4& view_matrix, const LightingBuffer& lights);
            void DrawTextures();
            void DrawAttributes();
            
            // Enable the attributes for passing them to the shader
            void EnableAttributes()
            {
                for(GLuint i = 0; i < ATTRIBUTE_COUNTER; ++i)
                {
                    glEnableVertexAttribArray(i);
                }
            }

            // When we already used this attributes, disable them
            void DisableAttributes()
            {
                for(GLuint i = 0; i < ATTRIBUTE_COUNTER; ++i)
                {
                    glDisableVertexAttribArray(i);
                }
            }

        };
    }

#endif