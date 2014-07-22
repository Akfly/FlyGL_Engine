/* ---------------------------------------------------------------------------
** Mesh.cpp
** Class that represents a static mesh (loaded from an .obj file). It also
** manages some data to pass it to the shader.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#include "Mesh.hpp"

// The OBJ Loader
#include "tinyobjloader\tiny_obj_loader.h"

// A little help indexing the data loaded from an obj
#include "objindexer\vboindexer.hpp"

namespace flygl
{
    // Loads the mesh from an .obj File
    //
    // path     The path route of the file
    void Mesh::LoadMesh(const std::string& path)
    {
        std::vector< glm::vec3 > _vertices;
        std::vector< glm::vec2 > _uvs;
        std::vector< glm::vec3 > _normals;
        std::vector< glm::vec3 > _tangents;
        std::vector< glm::vec3 > _bitangents;

        LoadMeshFromFile(path, _vertices, _uvs, _normals);                
        ComputeTangents (_vertices, _uvs, _normals, _tangents, _bitangents);
        
        // Index every data (vertices, uvs, normals, tangents and bitangents)
        indexVBO_TBN    (_vertices, _uvs, _normals, _tangents, _bitangents, 
                 indices, vertices,  uvs,  normals,  tangents,  bitangents);

        InitializeGLBuffers();
    }

    // Loads the shaders and Compile them.
    void Mesh::LoadShaders(const std::string& vertex_path, const std::string& fragment_path)
    {
        shaders.LoadVertexShader  (vertex_path  );
	    shaders.LoadFragmentShader(fragment_path);
    
        shaders.CompileShaders();
        shaders.UseThisShader ();
    }

    // Sets a texture for the mesh
    //
    // texture_path     The path route of the texture
    // uniform_name     The name of the uniform that has the texture on the shader
    void Mesh::SetTexture(const std::string& texture_path, const std::string& uniform_name)
    {
        int img_width, img_height, comp_num;
        unsigned char *data = stbi_load(texture_path.c_str(), &img_width, &img_height, &comp_num, 0);

        // Create one OpenGL texture
	    GLuint textureID;
	    glGenTextures(1, &textureID);
	
	    glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 
                        img_width, img_height, 
                        0, GL_RGB, GL_UNSIGNED_BYTE, 
                        data);

        // Free the data, we already have it stored
        stbi_image_free(data);

        // Set texture Parameters
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT              );
	    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT              );
	    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR              );
	    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	    glGenerateMipmap(GL_TEXTURE_2D);

        // Push the texture ID and its Uniform ID to the array
          textures.push_back(textureID);
        textureIDs.push_back(shaders.SetUniform(uniform_name));
    }

    // Initialize the required shader uniforms and store their IDs
    void Mesh::SetBasicUniforms()
    {
        // Matrix Uniforms
        matrixID       = shaders.SetUniform("MVP"        );
        viewMatrixID   = shaders.SetUniform("viewMatrix" );
        modelMatrixID  = shaders.SetUniform("modelMatrix");
                               
        // Light Uniforms
        lightPosID     = shaders.SetUniform("lightPos"      );
        lightColorID   = shaders.SetUniform("lightColor"    );
        lightPowerID   = shaders.SetUniform("lightPower"    );
        lightNumberID  = shaders.SetUniform("numberOfLights");
        modelView3x3ID = shaders.SetUniform("modelView3x3"  );
        
        // Motion Blur
        oldMVP_ID = shaders.SetUniform("oldMVP");
    }

    // Draws the mesh into the draw buffer
    void Mesh::Draw(const glm::mat4& projection_matrix, const glm::mat4& view_matrix, const LightingBuffer& lights)
    {
        shaders.UseThisShader();

        // Matrices
        MVP = projection_matrix * view_matrix * model_matrix;
        
        DrawUniforms(projection_matrix, view_matrix, lights);
        DrawTextures();

        EnableAttributes();
        DrawAttributes();
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        //             |Mode         |Count         |Type             |Array buff offset
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

        DisableAttributes();

        // We can store now the actual MVP as the previous one
        oldMVP = MVP;
    }

    // Loads the exact data from an obj (without indexing or anything)
    //
    // path         The path route of the Obj
    // _vertices    The array of vertices where we are going to store the data
    // _uvs         The array of uvs where we are going to store the data
    // _normals     The array of normals where we are going to store the data
    void Mesh::LoadMeshFromFile(const std::string path, 
        std::vector<glm::vec3>& _vertices, 
        std::vector<glm::vec2>& _uvs, 
        std::vector<glm::vec3>& _normals)
    {
        std::vector<tinyobj::shape_t> shapes;
        std::string err = tinyobj::LoadObj(shapes, path.c_str(), "");

        _vertices.resize(shapes[0].mesh.positions.size() / 3);
        for(size_t i = 0, j = 0; i < _vertices.size(); ++i, j+=3)
        {
            _vertices[i][0] = shapes[0].mesh.positions[j+0];
            _vertices[i][1] = shapes[0].mesh.positions[j+1];
            _vertices[i][2] = shapes[0].mesh.positions[j+2];
        }
        
        _uvs.resize(shapes[0].mesh.texcoords.size() / 2);
        for(size_t i = 0, j = 0; i < _uvs.size(); ++i, j += 2)
        {
            _uvs[i][0] = shapes[0].mesh.texcoords[j+0];
            _uvs[i][1] = shapes[0].mesh.texcoords[j+1] * -1;
        }

        _normals.resize(shapes[0].mesh.normals.size() / 3);
        for(size_t i = 0, j = 0; i < _normals.size(); ++i, j += 3)
        {
            _normals[i][0] = shapes[0].mesh.normals[j+0];
            _normals[i][1] = shapes[0].mesh.normals[j+1];
            _normals[i][2] = shapes[0].mesh.normals[j+2];
        }
    }

    // Creates the tangents and bitangents from the actual gathered data
    // (vertices, uvs and normals).
    // Tangents and Bitangents are used in the shader for applying the
    // normal map. This method is expensive and it's done on load only.
    //
    // _vertices    Position of each vertex
    // _uvs         Coordinates of the texture
    // _normals     Normal vertices loaded from the Obj
    // _tangents    Tangents vectors to the normal vectors. They must be perpendicular to their corresponding normals (think about an L)
    // _bitangents  The resulting tangent (or second tangent) from the Normal and Tangent.
    void Mesh::ComputeTangents(
    const std::vector<glm::vec3>& _vertices, 
    const std::vector<glm::vec2>& _uvs, 
    const std::vector<glm::vec3>& _normals,
          std::vector<glm::vec3>& _tangents, 
          std::vector<glm::vec3>& _bitangents)
    {
        for ( size_t i=0; i<_vertices.size(); i+=3)
        {
            // Shortcuts for vertices
            const glm::vec3& vertex0 = _vertices[i+0];
            const glm::vec3& vertex1 = _vertices[i+1];
            const glm::vec3& vertex2 = _vertices[i+2];
 
            // Shortcuts for UVs
            const glm::vec2 & uv0 = _uvs[i+0];
            const glm::vec2 & uv1 = _uvs[i+1];
            const glm::vec2 & uv2 = _uvs[i+2];
 
            // Edges of the triangle : postion delta
            const glm::vec3 deltaPos1 = vertex1-vertex0;
            const glm::vec3 deltaPos2 = vertex2-vertex0;
 
            // UV delta
            const glm::vec2 deltaUV1 = uv1-uv0;
            const glm::vec2 deltaUV2 = uv2-uv0;

            // Calculate them
            const float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            const glm::vec3 _tangent   = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
            const glm::vec3 _bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;

            // Finally, push them
            _tangents.push_back(_tangent);
            _tangents.push_back(_tangent);
            _tangents.push_back(_tangent);
 
            _bitangents.push_back(_bitangent);
            _bitangents.push_back(_bitangent);
            _bitangents.push_back(_bitangent);
        }

        //  Make the tangent perpendicular to the normal
        for (unsigned int i=0; i<_vertices.size(); i+=1 )
        {
	        const glm::vec3 & normal    = _normals   [i];
	        const glm::vec3 & bitangent = _bitangents[i];
		          glm::vec3 & tangent   = _tangents  [i];

	        // Gram-Schmidt orthogonalize
	        tangent = glm::normalize(tangent - normal * glm::dot(normal, tangent));
		
	        // Calculate handedness: If we have to invert the tangent
	        if (glm::dot(glm::cross(normal, tangent), bitangent) < 0.0f)
            {
		        tangent = tangent * -1.0f;
	        }
        }
    }

    // Once we have loaded from the .obj file, done every calculation, 
    // and indexed the data, we initialize the GL buffers with that data.
    void Mesh::InitializeGLBuffers()
    {
        glGenBuffers(1,              &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
                
        glGenBuffers(1,               &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER,  colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
                
        glGenBuffers(1,               &uvBuffer);
        glBindBuffer(GL_ARRAY_BUFFER,  uvBuffer);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
        
        glGenBuffers(1,               &normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER,  normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

        glGenBuffers(1, &tangentBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
        glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
 
        glGenBuffers(1, &bitangentBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, bitangentBuffer);
        glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
        
        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
    }

    // Pass every data to the shader.
    void Mesh::DrawUniforms(const glm::mat4& projection_matrix, const glm::mat4& view_matrix, const LightingBuffer& lights)
    {       
        glm::mat3 ModelView3x3Matrix = glm::mat3(view_matrix * model_matrix);

        // Lights
        glUniform3fv(lightPosID,    lights.numberOfLights, &lights.positionBuffer [0]);
        glUniform3fv(lightColorID,  lights.numberOfLights, &lights.colorBuffer    [0]);
        glUniform1fv(lightPowerID,  lights.numberOfLights, &lights.intensityBuffer[0]);
        glUniform1i (lightNumberID, lights.numberOfLights);
        
        //Set Basic Uniforms
        glUniformMatrix4fv(matrixID,       1, GL_FALSE, &MVP[0][0]               );
        glUniformMatrix4fv(viewMatrixID,   1, GL_FALSE, &view_matrix[0][0]       );
        glUniformMatrix4fv(modelMatrixID,  1, GL_FALSE, &model_matrix[0][0]      );
        glUniformMatrix4fv(oldMVP_ID,      1, GL_FALSE, &oldMVP[0][0]            );
        glUniformMatrix3fv(modelView3x3ID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
    }

    // Pass the textures to the shader.
    void Mesh::DrawTextures()
    {
        GLenum textCount = GL_TEXTURE0;
        for(size_t i = 0; i < textures.size(); ++i)
        {
		    glActiveTexture(textCount);
		    glBindTexture  (GL_TEXTURE_2D, textures[i]);
		    glUniform1i    (textureIDs[i], i);

            textCount++;
        }
    }

    // Pass the attribute data to the shaders
    void Mesh::DrawAttributes()
    {
        // Example:
        //                       |Attnum | Size |Type     |Normalized? |Stride |Array buff offset
        // glVertexAttribPointer(0,       3,    GL_FLOAT, GL_FALSE,    0,      (void*)0          );

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
                                                               
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
                                                               
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);           
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
                                                               
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);       
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
                                                               
        glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);      
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
                                                               
        glBindBuffer(GL_ARRAY_BUFFER, bitangentBuffer);    
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

}