#include "physics.hpp"
#include "GameObject/gameobject.hpp"

//extern ECSManager& manager;

void PhysicsSystem::Init()
{
}

void PhysicsSystem::Update(float dt, ECSManager& manager) 
{
	for (auto& entity : mEntities)
	{
		auto& transform = manager.GetComponent<SO::TransformComponent>(entity);
		auto& velocity = manager.GetComponent<MovementComponent>(entity).velocety;

		transform.position += glm::vec3{ 0.0f, -0.1f, 0.0f } *velocity * dt;
	}
}