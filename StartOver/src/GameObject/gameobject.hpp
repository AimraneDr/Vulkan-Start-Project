#pragma once

#include "ECS/ecs_manager.hpp"
#include "Components/transform.hpp"



namespace SO {
	class GameObject {
	public:
		static ECSManager Manager;
		Components::TransformComponent& transform;

		static GameObject createGameObject() {
			Entity newID = Manager.CreateEntity();
			return GameObject( newID );
		}

		//Note : the object created by this function have a copy of the transform component provided in the argument, not a ref to it
		static GameObject createGameObject(Components::TransformComponent _transfomr) {
			Entity newID = Manager.CreateEntity();
			return GameObject( newID, _transfomr );
		}


		//Copy
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;

		//Move
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		template<typename T>
		void AddComponent(T component) { Manager.AddComponent(id, component); };

		template<typename T>
		T& GetComponent() { return Manager.GetComponent<T>(id); };

		template<typename T>
		void RemoveComponent(T component) { Manager.RemoveComponent<T>(id); };

		const Entity getID() { return id; };
	private:
		GameObject(Entity id);
		GameObject(Entity id, Components::TransformComponent _transfomr);
		Entity id;
	};
}