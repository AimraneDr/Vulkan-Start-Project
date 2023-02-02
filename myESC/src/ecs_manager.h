#include "defines.h"
#include "components_manager.h"
#include "entity_manager.h"
#include "system_manager.h"

class ECSManager
{
public:
	void Init()
	{
		// Create pointers to each manager
		mComponentManager = std::make_unique<ComponentManager>();
		mEntityManager = std::make_unique<EntityManager>();
		mSystemManager = std::make_unique<SystemManager>();
	}


	// Entity methods
	EntityID CreateEntity()
	{
		return mEntityManager->createEntity();
	}

	void DestroyEntity(EntityID& entity)
	{
		mEntityManager->destroyEntity(entity);

		mComponentManager->EntityDestroyed(entity);

		mSystemManager->EntityDestroyed(entity);
	}


	// Component methods
	template<typename T>
	void RegisterComponent()
	{
		mComponentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(EntityID entity, T component)
	{
		mComponentManager->AddComponent<T>(entity, component);

		auto signature = mEntityManager->getComponentsMask(entity);
		signature |= mComponentManager->GetComponentType<T>();
		mEntityManager->setComponentsMask(entity, signature);

		mSystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(EntityID entity)
	{
		ComponentType type = mComponentManager->GetComponentType<T>();
		if (!mEntityManager->entityHasComponent(entity, type)) return;
		mComponentManager->RemoveComponent<T>(entity);

		auto signature = mEntityManager->getComponentsMask(entity);
		signature ^= type;
		mEntityManager->setComponentsMask(entity, signature);

		mSystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(EntityID entity)
	{
		return mComponentManager->GetComponent<T>(entity);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return mComponentManager->GetComponentType<T>();
	}


	// System methods
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		return mSystemManager->RegisterSystem<T>();
	}

	template<typename T>
	void SetSystemSignature(Mask64 signature)
	{
		mSystemManager->SetSignature<T>(signature);
	}

private:
	std::unique_ptr<ComponentManager> mComponentManager;
	std::unique_ptr<EntityManager> mEntityManager;
	std::unique_ptr<SystemManager> mSystemManager;
};