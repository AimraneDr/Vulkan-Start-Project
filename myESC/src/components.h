#pragma once
#include "defines.h"
#include <cassert>
#include <unordered_map>
#include <array>



class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(EntityID id) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray
{
public:
	void InsertData(EntityID id, T component)
	{
		assert(mEntityToIndexMap.find(id) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

		// Put new entry at end and update the maps
		size_t newIndex = mSize;
		mEntityToIndexMap[id] = newIndex;
		mIndexToEntityMap[newIndex] = id;
		mComponentArray[newIndex] = component;
		mSize++;
	}

	void RemoveData(EntityID id)
	{
		assert(mEntityToIndexMap.find(id) != mEntityToIndexMap.end() && "Removing non-existent component.");

		// Copy element at end into deleted element's place to maintain density
		size_t indexOfRemovedEntity = mEntityToIndexMap[id];
		size_t indexOfLastElement = mSize - 1;
		mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

		// Update map to point to moved spot
		EntityID entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
		mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		mEntityToIndexMap.erase(id);
		mIndexToEntityMap.erase(indexOfLastElement);

		mSize--;
	}

	T& GetData(EntityID entity)
	{
		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");

		// Return a reference to the entity's component
		return mComponentArray[mEntityToIndexMap[entity]];
	}

	void EntityDestroyed(EntityID id) override
	{
		if (mEntityToIndexMap.find(id) != mEntityToIndexMap.end())
		{
			// Remove the entity's component if it existed
			RemoveData(id);
		}
	}

private:
	// The packed array of components (of generic type T),
	// set to a specified maximum amount, matching the maximum number
	// of entities allowed to exist simultaneously, so that each entity
	// has a unique spot.
	std::array<T, MAX_ENTITIES> mComponentArray;

	// Map from an entity ID to an array index.
	std::unordered_map<EntityID, size_t> mEntityToIndexMap;

	// Map from an array index to an entity ID.
	std::unordered_map<size_t, EntityID> mIndexToEntityMap;

	// Total size of valid entries in the array.
	size_t mSize;
};