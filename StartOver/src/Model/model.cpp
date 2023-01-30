#include "model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "Utils/utils.hpp"

#include <unordered_map>
#include <cstring>
#include <stdexcept>

namespace std {
	template<>
	struct hash<SO::GameModel::Vertex> {
		size_t operator()(SO::GameModel::Vertex const& vertex)const {
			size_t seed = 0;
			SO::hashCombine(seed, vertex.pos, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace SO {

	GameModel::GameModel(RendererDevice& device, const std::vector<Vertex>& vertices, std::vector<uint32_t> indices)
		:rDevice{device}
	{
		createVertexBuffers(vertices);
		if (indices.size() > 0) {
			hasIndexBuffer = true;
			createIndexBuffers(indices);
		}
		else { hasIndexBuffer = false; }
	}

	GameModel::~GameModel() {}

	std::unique_ptr<GameModel> GameModel::loadModelFromFile(RendererDevice& device, const std::string& filePath) {
		std::vector<Vertex> _vertices{};
		std::vector<uint32_t> _indices{};

		loadModel(filePath, _vertices, _indices);

		return std::make_unique<GameModel>(device, _vertices, _indices);
	}

	std::vector<VkVertexInputBindingDescription> GameModel::Vertex::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> descriptions(1);
		descriptions[0].binding = 0;
		descriptions[0].stride = sizeof(Vertex);
		descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return descriptions;
	}

	std::vector<VkVertexInputAttributeDescription> GameModel::Vertex::getAttributeDescriptions() {
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

	void GameModel::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		if (hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void GameModel::draw(VkCommandBuffer commandBuffer) {
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}

	void GameModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex Count must be greater or equale to 3!");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		RendererBuffer stagingBuffer{ 
			rDevice, 
			vertexSize, 
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		vertexBuffer = std::make_unique<RendererBuffer>(
			rDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		rDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}
	
	void GameModel::createIndexBuffers(const std::vector<uint32_t>& indices) {
		indexCount = static_cast<uint32_t>(indices.size());
		assert(indexCount >= 3 && "Index Count must be greater or equale to 3!");
		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		uint32_t indexSize = sizeof(indices[0]);

		RendererBuffer stagingBuffer{
			rDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		indexBuffer = std::make_unique<RendererBuffer>(
			rDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
		rDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}


	void GameModel::loadModel(const std::string& path, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
			throw std::runtime_error("Failed to Load Object : " + path + "\n\twarn : " + warn + "\n\terr : " + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};
				if (index.vertex_index >= 0) {
					vertex.pos = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};

				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}
				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

}