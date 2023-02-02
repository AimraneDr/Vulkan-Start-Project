#pragma once

#include "defines.h"
#include <array>
#include <vector>
#include <cassert>

static class EntityManager {
public:

	EntityManager() {
		for (int i = 0; i < sizeof(componentMasks) / sizeof(Mask64); i++) {
			componentMasks[i] = INVALID_COMPONENT_MASK;
		}
	}

	EntityID createEntity() {
		assert((nextID < MAX_ENTITIES || freeIDs.size() > 0) && "too many entities in existence");
		EntityID newID;
		if (freeIDs.size() > 0) {
			newID = freeIDs.back();
			freeIDs.pop_back();
		}else
			newID = nextID++;
		componentMasks[newID] = 0;
		return newID;
	}
	void destroyEntity(EntityID& id) {
		assert(id < MAX_ENTITIES && "entity's id out of range");
		freeIDs.push_back(id);
		componentMasks[id] = INVALID_COMPONENT_MASK;
		id = INVALID_ID;
	}

	Mask64 getComponentsMask(EntityID id) {
		assert(id < MAX_ENTITIES && "entity's id out of range");
		return componentMasks[id];
	}
	void setComponentsMask(EntityID id, Mask64 cMask) {
		assert(id < MAX_ENTITIES && "entity's id out of range");
		componentMasks[id] = cMask;
	}
	Mask64 resetComponentsMask(EntityID id) {
		assert(id < MAX_ENTITIES && "entity's id out of range");
		return componentMasks[id] = 0;
	}

	//return 0 if not found and the component_id if found in the mask
	bool entityHasComponent(EntityID entity_id, ComponentType component_type) {
		Mask64 mask = componentMasks[entity_id];
		return ~(((~mask) | component_type) ^ mask);
	}
private:
	ID nextID{ 0 };
	ID nextFreeID{ 0 };
	Mask64 componentMasks[MAX_ENTITIES];
	std::vector<EntityID> freeIDs;
};