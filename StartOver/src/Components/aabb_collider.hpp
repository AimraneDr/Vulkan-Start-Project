#pragma once
#include "Renderer/device.hpp"
#include "Renderer/buffer.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <stdlib.h> 

namespace Components {
	struct AABBCollider {
		glm::vec3 center{.0f};
		glm::vec3 halfSize{ 1.f,1.f,1.f };

		//TODO : remove the device from this component
		AABBCollider(){}
		AABBCollider(glm::vec3 center, glm::vec3 halfSize = glm::vec3(1.f,1.f,1.f ))
			:center{ center }, halfSize{ halfSize }
		{

		}


		bool isPointEntersectWithCollider(float x, float y, float z) {
			if (abs(center.x - x) > halfSize.x) return false;
			if (abs(center.y - y) > halfSize.y) return false;
			if (abs(center.z - z) > halfSize.z) return false;
			return true;
		}

		bool isAABBentersectWithAABB(AABBCollider aabb)
		{
			if (abs(center.x - aabb.center.x) > (halfSize.x + aabb.halfSize.x)) return false;
			if (abs(center.y - aabb.center.y) > (halfSize.y + aabb.halfSize.y)) return false;
			if (abs(center.z - aabb.center.z) > (halfSize.z + aabb.halfSize.z)) return false;
			return true;
		}

		// Transform AABB a by the matrix m and translation t,
		// find maximum extents, and store result into AABB b.
		void UpdateAABB(glm::mat3 m, glm::vec3 t)
		{
			for (int i = 0; i < 3; i++) {
				center[i] = t[i];
				halfSize[i] = 0.0f;
				for (int j = 0; j < 3; j++) {
					center[i] += m[i][j] * center[j];
					halfSize[i] += abs(m[i][j]) * halfSize[j];
				}
			}
		}
		
	};
}