#pragma once
#include "ECS/system.hpp"
#include <glm/glm.hpp>
#include "Renderer/device.hpp"

//Temporary Component
struct MovementComponent {
	glm::vec3 velocety{};
};

namespace Systems {
	class PhysicsSystem : public System
	{
	public:
		void Init();

		//TODO remove the dependency on render device from this system
		void Update(float dt);
	};
}