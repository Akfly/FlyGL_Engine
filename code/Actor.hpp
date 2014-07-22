/* ---------------------------------------------------------------------------
** Actor.hpp
** This class represents an element in the world, it could be a Mesh, a Camera,
** a Light, a Sound, or wathever you may think of. It just has the required
** transformations: Position, Rotation and Scale.
**
** Author: Fly - Ruben Negredo
** -------------------------------------------------------------------------*/

#ifndef ACTOR_HEADER
#define ACTOR_HEADER

	#include <vector>
	#include <cassert>

    // GLM
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>

    namespace flygl
    {
        class Actor
        {
        protected:
            glm::mat4 model_matrix;

			//World position, rotation, scale
			glm::vec3 pos;
			glm::vec3 rot;
			glm::vec3 sc;
            
        public:

			// Constructor
			Actor()
			{
				pos.x = 0;
				pos.y = 0;
				pos.z = 0;

				rot.x = 0;
				rot.y = 0;
				rot.z = 0;

				sc.x = 1;
				sc.y = 1;
				sc.z = 1;
			}

			// Sets the transformation buffer
			virtual void Update()
			{
                glm::mat4 identity;

                model_matrix = glm::scale     (identity, sc);
                model_matrix = glm::rotate    (model_matrix, rot.x, glm::vec3(1.f, 0.f, 0.f));
                model_matrix = glm::rotate    (model_matrix, rot.y, glm::vec3(0.f, 1.f, 0.f));
                model_matrix = glm::rotate    (model_matrix, rot.z, glm::vec3(0.f, 0.f, 1.f));
                model_matrix = glm::translate (model_matrix, pos);
			}

            // Returns the actual transformation matrix
            glm::mat4 GetMatrix() const
            {
                return model_matrix;
            }

            // Give a new angles
			void SetRotation(const float& _x, const float& _y, const float& _z)
			{
				rot.x = _x;
				rot.y = _y;
				rot.z = _z;
			}

            // Set a new position
			void SetPosition(const float& _x, const float& _y, const float& _z)
			{
				pos.x = _x;
				pos.y = _y;
				pos.z = _z;
			}

            // Change the scale into the new one
			void SetScale(const float& _x, const float& _y, const float& _z)
			{
				sc.x = _x;
				sc.y = _y;
				sc.z = _z;
			}

            // Change the scale into the new one
			void SetScale(const float& value)
			{
				sc.x = value;
				sc.y = value;
				sc.z = value;
			}

            // Give a new angles
            void SetRotation(const glm::vec3& new_rot)
			{
				rot.x = new_rot.x;
				rot.y = new_rot.y;
				rot.z = new_rot.z;
			}

            // Set a new position
			void SetPosition(const glm::vec3& new_pos)
			{
				pos.x = new_pos.x;
				pos.y = new_pos.y;
				pos.z = new_pos.z;
			}

            // Change the scale into the new one
			void SetScale(const glm::vec3& new_scale)
			{
				sc.x = new_scale.x;
				sc.y = new_scale.y;
				sc.z = new_scale.z;
			}

            // Returns the actual position
            glm::vec3 GetPosition() const
			{
				return pos;
			}

            // Returns the actual rotation angles
			glm::vec3 GetRotation() const
			{
				return rot;
			}

            // Returns the actual scale
			glm::vec3 GetScale() const
			{
				return sc;
			}

            // Moves from the actual position into a new one
			void Move(const float& _x, const float& _y, const float& _z)
			{
				pos.x += _x;
				pos.y += _y;
				pos.z += _z;
			}

            // Adds a rotation to the actual one
			void Rotate(const float& _x, const float& _y, const float& _z)
			{
				rot.x += _x;
				rot.y += _y;
				rot.z += _z; 
			}

            // Add a scale factor to the actual one
			void SumScale(const float& value)
			{
				sc.x += value;
				sc.y += value;
				sc.z += value;
			}

            // Add a scale factor to the actual one
			void SumScale(const float& _x, const float& _y, const float& _z)
			{
				sc.x += _x;
				sc.y += _y;
				sc.z += _z;
			}

            // Moves from the actual position into a new one
            void Move(const glm::vec3& pos_offset)
			{
				pos.x += pos_offset.x;
				pos.y += pos_offset.y;
				pos.z += pos_offset.z;
			}

            // Adds a rotation to the actual one
			void Rotate(const glm::vec3& rot_offset)
			{
				rot.x += rot_offset.x;
				rot.y += rot_offset.y;
				rot.z += rot_offset.z; 
			}

            // Add a scale factor to the actual one
            void SumScale(const glm::vec3& scale_offset)
			{
				sc.x += scale_offset.x;
				sc.y += scale_offset.y;
				sc.z += scale_offset.z;
			}
        };
    }

#endif
