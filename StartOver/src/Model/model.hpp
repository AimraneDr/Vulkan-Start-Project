#pragma once

#include "Renderer/device.hpp"
#include "Renderer/buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


#include <vector>
#include <memory>

namespace SO {
	class GameModel {
	public:

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



		GameModel(RendererDevice& device, const std::vector<Vertex>& vertices, std::vector<uint32_t> indices = {});
		~GameModel();

		GameModel(const GameModel&) = delete;
		GameModel &operator=(const GameModel &) = delete;

		static std::unique_ptr<GameModel> loadModelFromFile(RendererDevice& device, const std::string& filePath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		RendererDevice& rDevice;

		std::unique_ptr<RendererBuffer> vertexBuffer;
		uint32_t vertexCount;
		
		bool hasIndexBuffer;
		std::unique_ptr<RendererBuffer> indexBuffer;
		uint32_t indexCount;
		
		static void loadModel(const std::string& path, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t> &indices);
	};
}