#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Components {
	struct TransformComponent {
	public:
		glm::vec3 position;
		glm::vec3 scale{ 1.0f,1.0f,1.0f };
		glm::vec3 rotation;


		//helper functions : should not be in the system since it may be needed for all systems that runs on this component

		//TransformComponent& getParent() { return parent; };
		//void setParent();

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		 // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		 // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();

		glm::mat3 normalMat();

	};
}