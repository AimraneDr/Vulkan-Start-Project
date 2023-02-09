#include <iostream>
#include "physics.hpp"
#include "GameObject/gameobject.hpp"
#include "Components/components.h"
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
			auto& collider = SO::GameObject::Manager.GetComponent<Components::AABBCollider>(entity);
			if (!collider.isPointEntersectWithCollider(1.5f, 0.f, 1.f)) {
				transform.position += glm::vec3{ 0.0f, -0.1f, 0.0f } *velocity * dt;
				collider.position = transform.position;
			}
		}
	}

}