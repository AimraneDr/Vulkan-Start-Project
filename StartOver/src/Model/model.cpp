#include "model.hpp"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

//#include "Utils/utils.hpp"

#include "Renderer/device.hpp"

#include <stdexcept>
#include <iostream>



namespace SO {

	std::vector<VkVertexInputBindingDescription> Vertex::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> descriptions(1);
		descriptions[0].binding = 0;
		descriptions[0].stride = sizeof(Vertex);
		descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return descriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> descriptions(4);
		descriptions[0].binding = 0;
		descriptions[0].location = 0;
		descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		descriptions[0].offset = offsetof(Vertex, pos);

		descriptions[1].binding = 0;
		descriptions[1].location = 1;
		descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		descriptions[1].offset = offsetof(Vertex, color);

		descriptions[2].binding = 0;
		descriptions[2].location = 2;
		descriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		descriptions[2].offset = offsetof(Vertex, normal);

		descriptions[3].binding = 0;
		descriptions[3].location = 3;
		descriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		descriptions[3].offset = offsetof(Vertex, uv);

		return descriptions;
	}




	void Model::createVertexBuffers() {
		auto vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex Count must be greater or equale to 3!");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		RendererBuffer stagingBuffer{
			*RendererDevice::globalDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		vertexBuffer = std::make_shared<SO::RendererBuffer>(
			*RendererDevice::globalDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		RendererDevice::globalDevice->copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
		std::cout << "A call for 'CreatVertxBuffer'\n";
	}

	void Model::createIndexBuffers() {
		auto indexCount = static_cast<uint32_t>(indices.size());
		assert(indexCount >= 3 && "Index Count must be greater or equale to 3!");
		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		uint32_t indexSize = sizeof(indices[0]);

		RendererBuffer stagingBuffer{
			*RendererDevice::globalDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		indexBuffer = std::make_shared<SO::RendererBuffer>(
			*RendererDevice::globalDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
		RendererDevice::globalDevice->copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}

}