#pragma once
#include <glm/glm.hpp>
namespace Components {
	struct AABBCollider {
		glm::vec3 position{ 0.f,0.f, 0.f };
		float width{ 1.f };
		float height{ 1.f };
		float length{ 1.f };
		bool isPointEntersectWithCollider(float x, float y, float z) {
			float
				Xmin = position.x - (width / 2),
				Xmax = position.x + (width / 2),
				Ymin = position.y - (height / 2),
				Ymax = position.y + (height / 2),
				Zmin = position.z - (length / 2),
				Zmax = position.z + (length / 2);

			return{
				x >= Xmin &&
				x <= Xmax &&
				y >= Ymin &&
				y <= Ymax &&
				z >= Zmin &&
				z <= Zmax
			};
		}
	};
}