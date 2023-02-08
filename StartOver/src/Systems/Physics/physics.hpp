#pragma once
#include "ECS/system.hpp"
#include <glm/glm.hpp>

//Temporary Component
struct MovementComponent {
	glm::vec3 velocety{};
};

namespace Systems {
	class PhysicsSystem : public System
	{
	public:
		void Init();

		void Update(float dt);
	};
}