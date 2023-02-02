#pragma once
#define MAX_COMPONENT_TYPES sizeof(Mask64) * 8
#define MAX_ENTITIES 10000

typedef unsigned long long ID;
//typedef std::bitset<MAX_COMPONENTS> ComponentMask;
typedef unsigned long long Mask64;

using EntityID = ID; 
using ComponentID = ID;
using ComponentType = unsigned char;

//#define INVALID_ENTITY_ID 1 << 63
#define INVALID_COMPONENT_MASK 1 << 62
#define INVALID_ID -1