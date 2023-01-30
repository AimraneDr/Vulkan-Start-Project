#pragma once

#include "Model/model.hpp"
#include "Components/pointlight_component.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace SO {

	struct TransformComponent {
		glm::vec3 position;
		glm::vec3 scale{ 1.0f,1.0f,1.0f };
		glm::vec3 rotation;

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		 // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		 // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		
		glm::mat3 normalMat();
	};

	class GameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, GameObject>;

		std::shared_ptr<GameModel> model{};
		glm::vec3 color{};
		//Components
		TransformComponent transform{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr;

		static GameObject createGameObject() {
			static id_t current_id = 0;
			return GameObject{ current_id++ };
		}
		static GameObject createPointLightGameObject(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

		//Copy
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;

		//Move
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		const id_t getID() { return id; }
	private:
		GameObject(id_t id) : id{ id } {}
		id_t id;
	};
}