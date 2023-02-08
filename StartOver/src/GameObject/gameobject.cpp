#include "gameobject.hpp"

#include <glm/glm.hpp>

namespace SO {
	
	ECSManager GameObject::Manager;

	Components::TransformComponent defaultT = Components::TransformComponent{
			{.0f,.0f,.0f},
			{1.f,1.f,1.f},
			{.0f,.0f,.0f} };

	GameObject::GameObject(Entity id)
		:id{ id },transform{defaultT}
	{
		Manager.AddComponent(id, transform);
		transform = Manager.GetComponent<Components::TransformComponent>(id);
	}

	GameObject::GameObject(Entity id, Components::TransformComponent _transfomr)
		:id{ id }, transform{ _transfomr }
	{
		transform = Components::TransformComponent{
				_transfomr.position,
				_transfomr.scale,
				_transfomr.rotation
		};
		Manager.AddComponent(id,transform);
		transform = Manager.GetComponent<Components::TransformComponent>(id);
	}

}