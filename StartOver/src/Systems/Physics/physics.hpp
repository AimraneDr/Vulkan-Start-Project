#pragma once
#include "ECS/system.hpp"
#include "ECS/ecs_manager.hpp"
#include <glm/glm.hpp>

struct MovementComponent {
	glm::vec3 velocety{};
};

class PhysicsSystem : public System
{
public:
	void Init();

	void Update(float dt, ECSManager& manager);
};
