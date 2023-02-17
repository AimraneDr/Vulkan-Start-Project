#pragma once
#include "Renderer/buffer.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace SO {
	struct Vertex {
		glm::vec3 pos{};
		glm::vec3 color{};
		glm::vec3 normal{};
		glm::vec2 uv{};

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		bool operator==(const Vertex& other) const {
			return pos == other.pos && color == other.color && normal == other.normal && uv == other.uv;
		}
	};

	struct Model {
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};

		std::shared_ptr<RendererBuffer> vertexBuffer;

		bool hasIndexBuffer{false};
		std::shared_ptr<RendererBuffer> indexBuffer;


		void createVertexBuffers();
		void createIndexBuffers();
	};
	

}