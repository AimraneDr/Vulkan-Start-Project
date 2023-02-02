#include <stdio.h>
#include <bitset>
#include <vector>
#include <iostream>
#include <chrono>
#include <random>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "src/ecs_manager.h"

struct Gravity
{
	glm::vec3 force;
};

struct RigidBody
{
	glm::vec3 velocity;
	glm::vec3 acceleration;
};

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};
ECSManager gameManager;

struct PhysicsSystem : public System {

	void Update(float dt)
	{
		for (auto const& entity : mEntities)
		{
			auto& rigidBody = gameManager.GetComponent<RigidBody>(entity);
			auto& transform = gameManager.GetComponent<Transform>(entity);
			auto const& gravity = gameManager.GetComponent<Gravity>(entity);

			transform.position += rigidBody.velocity * dt;

			rigidBody.velocity += gravity.force * dt;
		}
	}
};


int main()
{
	gameManager.Init();

	gameManager.RegisterComponent<Gravity>();
	gameManager.RegisterComponent<RigidBody>();
	gameManager.RegisterComponent<Transform>();

	auto physicsSystem = gameManager.RegisterSystem<PhysicsSystem>();

	gameManager.SetSystemSignature<PhysicsSystem>(gameManager.GetComponentType<Gravity>() | gameManager.GetComponentType<RigidBody>() | gameManager.GetComponentType<Transform>());

	std::vector<EntityID> entities(MAX_ENTITIES);

	std::default_random_engine generator;
	std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
	std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
	std::uniform_real_distribution<float> randScale(3.0f, 5.0f);
	std::uniform_real_distribution<float> randGravity(-10.0f, -1.0f);

	float scale = randScale(generator);

	for (auto& entity : entities)
	{
		entity = gameManager.CreateEntity();

		gameManager.AddComponent(
			entity,
			Gravity{ glm::vec3{0.0f, randGravity(generator), 0.0f} });

		gameManager.AddComponent(
			entity,
			RigidBody{
				glm::vec3{0.0f, 0.0f, 0.0f},
				glm::vec3{0.0f, 0.0f, 0.0f}
			});

		gameManager.AddComponent(
			entity,
			Transform{
				glm::vec3{randPosition(generator), randPosition(generator), randPosition(generator)},
				glm::vec3{randRotation(generator), randRotation(generator), randRotation(generator)},
				glm::vec3{scale, scale, scale}
			});
	}

	float dt = 0.0f;
	
	//should be in the game loop
	physicsSystem->Update(dt);

	for (auto& entity : entities)
	{
		gameManager.RemoveComponent<Transform>(entity);

		gameManager.RemoveComponent<RigidBody>(entity);

		gameManager.RemoveComponent<Gravity>(entity);
		
		gameManager.DestroyEntity(entity);

		//std::cout << entity << "destroyed\n";
	}

	//std::cout << entities.size() << " was in existence\n";


	return 1;
}
