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
	struct hash<SO::Vertex> {
		size_t operator()(SO::Vertex const& vertex)const {
			size_t seed = 0;
			SO::hashCombine(seed, vertex.pos, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

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

	GameModel::GameModel(RendererDevice& device)
		:rDevice{ device } 
	{
		
	}


	void GameModel::createVertexBuffers(const std::vector<Vertex>& vertices, Model& out) {
		out.vertexCount = static_cast<uint32_t>(vertices.size());
		assert(out.vertexCount >= 3 && "Vertex Count must be greater or equale to 3!");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * out.vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		RendererBuffer stagingBuffer{
			rDevice,
			vertexSize,
			out.vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		out.vertexBuffer = std::make_shared<SO::RendererBuffer>(
			rDevice,
			vertexSize,
			out.vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		rDevice.copyBuffer(stagingBuffer.getBuffer(), out.vertexBuffer->getBuffer(), bufferSize);
	}
	
	void GameModel::createIndexBuffers(const std::vector<uint32_t>& indices, Model& out) {
		out.indexCount = static_cast<uint32_t>(indices.size());
		assert(out.indexCount >= 3 && "Index Count must be greater or equale to 3!");
		VkDeviceSize bufferSize = sizeof(indices[0]) * out.indexCount;
		uint32_t indexSize = sizeof(indices[0]);

		RendererBuffer stagingBuffer{
			rDevice,
			indexSize,
			out.indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		out.indexBuffer = std::make_shared<SO::RendererBuffer>(
			rDevice,
			indexSize,
			out.indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
		rDevice.copyBuffer(stagingBuffer.getBuffer(), out.indexBuffer->getBuffer(), bufferSize);
	}


	void GameModel::loadModel(const std::string& path, Model& m) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
			throw std::runtime_error("Failed to Load Object : " + path + "\n\twarn : " + warn + "\n\terr : " + err);
		}
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};

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
		m.vertexCount = vertices.size();
		createVertexBuffers(vertices, m);
		m.indexCount = indices.size();
		if (m.indexCount > 0) {
			m.hasIndexBuffer = true;
			createIndexBuffers(indices, m);
		}else{ m.hasIndexBuffer = false; }
	}

	Model GameModel::loadModel(const std::string& path) {
		std::shared_ptr<Model> m = std::make_shared<Model>();
		loadModel(path, *m);
		return *m.get();
	}
}