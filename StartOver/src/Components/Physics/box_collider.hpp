#pragma once
#include "Renderer/device.hpp"
#include "Renderer/buffer.hpp"
#include "Model/model.hpp"
#include "Resources/resources_loader.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <stdlib.h> 

namespace Components {
	struct BoxCollider {
		glm::vec3 center{.0f};
		glm::vec3 halfSize{ 1.f,1.f,1.f };
		glm::vec3 rotation{};
		SO::Model mesh{};

		//TODO : remove the device from this component
		BoxCollider(){}
		BoxCollider(glm::vec3 center, glm::vec3 halfSize = glm::vec3(1.f,1.f,1.f ))
			:center{ center }, halfSize{ halfSize }
		{
			mesh = {};
			SO::Loader::LoadObjFormat("C:\\VStudio\\StartOver\\StartOver\\assets\\models\\colored_cube.obj", mesh);
			//resize
			for (auto& vert : mesh.vertices) {
				vert.pos *= halfSize;
			}
			mesh.createVertexBuffers();
			mesh.createIndexBuffers();
		}

		glm::mat4 mat4() {
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
				{
					halfSize.x * (c1 * c3 + s1 * s2 * s3),
					halfSize.x * (c2 * s3),
					halfSize.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					halfSize.y * (c3 * s1 * s2 - c1 * s3),
					halfSize.y * (c2 * c3),
					halfSize.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					halfSize.z * (c2 * s1),
					halfSize.z * (-s2),
					halfSize.z * (c1 * c2),
					0.0f,
				},
				{center.x, center.y, center.z, 1.0f}
			};
		}//mat4

		bool isPointEntersectWithCollider(float x, float y, float z) {
			if (abs(center.x - x) > halfSize.x) return false;
			if (abs(center.y - y) > halfSize.y) return false;
			if (abs(center.z - z) > halfSize.z) return false;
			return true;
		}

		bool isAABBentersectWithAABB(BoxCollider aabb)
		{
			if (abs(center.x - aabb.center.x) > (halfSize.x + aabb.halfSize.x)) return false;
			if (abs(center.y - aabb.center.y) > (halfSize.y + aabb.halfSize.y)) return false;
			if (abs(center.z - aabb.center.z) > (halfSize.z + aabb.halfSize.z)) return false;
			return true;
		}

		// Transform AABB a by the matrix m and translation t,
		// find maximum extents, and store result into AABB b.
		void UpdateBoxCollider(glm::mat3 m, glm::vec3 t)
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