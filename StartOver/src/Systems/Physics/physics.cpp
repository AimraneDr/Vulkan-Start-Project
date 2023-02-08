#include "physics.hpp"
#include "GameObject/gameobject.hpp"

namespace Systems {

	void PhysicsSystem::Init()
	{
	}

	void PhysicsSystem::Update(float dt)
	{
		for (auto& entity : mEntities)
		{
			auto& transform = SO::GameObject::Manager.GetComponent<Components::TransformComponent>(entity);
			auto& velocity = SO::GameObject::Manager.GetComponent<MovementComponent>(entity).velocety;

			transform.position += glm::vec3{ 0.0f, -0.1f, 0.0f } *velocity * dt;
		}
	}

}