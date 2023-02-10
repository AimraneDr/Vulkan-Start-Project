#pragma once
#include "Renderer/device.hpp"
#include "Renderer/buffer.hpp"

#include <glm/glm.hpp>
#include <memory>
namespace Components {
	struct AABBCollider {
		glm::vec3 position{.0f};
		float width{1};
		float height{1};
		float length{1};

		//TODO : remove the device from this component
		AABBCollider(){}
		AABBCollider(glm::vec3 position, float width, float height, float length, SO::RendererDevice& device)
			:position{position}, width{width},height{height},length{length}
		{
			createVertexBuffers(device);
			createIndexBuffers(device);
		}

		uint32_t vertexCount{ 0 };
		uint32_t indexCount{ 0 };
		VkBuffer getVertexBuffer() {
			return vertBuffer->getBuffer();
		}
		VkBuffer getIndexBuffer() {
			return indBuffer->getBuffer();
		}


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
		bool isAABBentersectWithAABB(AABBCollider aabb) {
			float
				AminX = position.x - (width / 2),
				AmaxX = position.x + (width / 2),
				AminY = position.y - (height / 2),
				AmaxY = position.y + (height / 2),
				AminZ = position.z - (length / 2),
				AmaxZ = position.z + (length / 2);
			float
				BminX = aabb.position.x - (aabb.width / 2),
				BmaxX = aabb.position.x + (aabb.width / 2),
				BminY = aabb.position.y - (aabb.height / 2),
				BmaxY = aabb.position.y + (aabb.height / 2),
				BminZ = aabb.position.z - (aabb.length / 2),
				BmaxZ = aabb.position.z + (aabb.length / 2);
			return (
				AminX <= BmaxX &&
				AmaxX >= BminX &&
				AminY <= BmaxY &&
				AmaxY >= BminY &&
				AminZ <= BmaxZ &&
				AmaxZ >= BminZ
				);
		}

	private:
		std::shared_ptr<SO::RendererBuffer> vertBuffer, indBuffer;
		std::vector<glm::vec3> vertices = {
			glm::vec3 { .5f, -.5f, .5f },
			glm::vec3 { .5f, -.5f, -.5f },
			glm::vec3 { -.5f, -.5f, -.5f },
			glm::vec3 { -.5f, -.5f, .5f },
			glm::vec3 { .5f, .5f, .5f },
			glm::vec3 { .5f, .5f, -.5f },
			glm::vec3 { -.5f, .5f, -.5f },
			glm::vec3 { -.5f, .5f, .5f }
		};
		std::vector<uint32_t> indices = {
			0,1,	1,2,
			2,3,	3,0,
			4,5,	5,6,
			6,7,	7,4,
			0,4,	1,5,
			2,6,	3,7
		};

		std::vector<glm::vec3> resize(float w, float h, float l) {
			std::vector<glm::vec3> verts;
			for (int i = 0; i < 8; i++) {
				verts.push_back(
					{
						vertices[i].x * w,
						vertices[i].y * h,
						vertices[i].z * l
					}
				);
			}
			return verts;
		}


		void createVertexBuffers(SO::RendererDevice& rDevice) {
			std::vector<glm::vec3> verts = resize(width, height, length);
			vertexCount = static_cast<uint32_t>(verts.size());
			assert(vertexCount >= 2 && "Vertex Count must be greater or equale to 3!");
			VkDeviceSize bufferSize = sizeof(verts[0]) * vertexCount;
			uint32_t vertexSize = sizeof(verts[0]);

			SO::RendererBuffer stagingBuffer{
				rDevice,
				vertexSize,
				vertexCount,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			};
			stagingBuffer.map();
			stagingBuffer.writeToBuffer((void*)verts.data());

			vertBuffer = std::make_shared<SO::RendererBuffer>(
				rDevice,
				vertexSize,
				vertexCount,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);

			rDevice.copyBuffer(stagingBuffer.getBuffer(), vertBuffer->getBuffer(), bufferSize);
		}

		void createIndexBuffers(SO::RendererDevice& rDevice) {
			indexCount = static_cast<uint32_t>(indices.size());
			assert(indexCount >= 2 && "Index Count must be greater or equale to 3!");
			VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
			uint32_t indexSize = sizeof(indices[0]);

			SO::RendererBuffer stagingBuffer{
				rDevice,
				indexSize,
				indexCount,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			};
			stagingBuffer.map();
			stagingBuffer.writeToBuffer((void*)indices.data());

			indBuffer = std::make_shared<SO::RendererBuffer>(
				rDevice,
				indexSize,
				indexCount,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);
			rDevice.copyBuffer(stagingBuffer.getBuffer(), indBuffer->getBuffer(), bufferSize);
		}

	};
}