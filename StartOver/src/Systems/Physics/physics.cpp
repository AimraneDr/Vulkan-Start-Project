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
		static auto col = Components::BoxCollider{
			{ 0.f, 0.f, 0.f },
			{12.f, 0.f, 12.f}
		};

		for (auto& entity : mEntities)
		{
			auto& transform = SO::GameObject::Manager.GetComponent<Components::TransformComponent>(entity);
			auto& velocity = SO::GameObject::Manager.GetComponent<MovementComponent>(entity).velocety;
			auto& collider = SO::GameObject::Manager.GetComponent<Components::BoxCollider>(entity);
			transform.rotation.x += glm::two_pi<float>() / 12 * dt;
			if (!collider.isAABBentersectWithAABB(col)) {
				transform.position += glm::vec3{ 0.0f, -0.1f, 0.0f } *velocity * dt;
			}

			transform.position += glm::vec3{ 0.0f, 0.0f, 0.1f } *velocity * dt;
			collider.center = transform.position;

		}
	}

}