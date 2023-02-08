#pragma once

#include "Renderer/device.hpp"
#include "Renderer/buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


#include <vector>
#include <memory>

namespace SO {
	struct Model {
		std::shared_ptr<RendererBuffer> vertexBuffer;
		uint32_t vertexCount{0};

		bool hasIndexBuffer{false};
		std::shared_ptr<RendererBuffer> indexBuffer;
		uint32_t indexCount{0};
	};
	
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

	class GameModel {
	public:
		GameModel(RendererDevice& device);
		~GameModel() {};

		//copy
		GameModel(const GameModel&) = delete;
		GameModel &operator=(const GameModel &) = delete;

		void loadModel(const std::string& path, Model& m);
		Model loadModel(const std::string& path);
		static void loadModel(const std::string& path, Model& m, RendererDevice& rDevice);
		static Model loadModel(const std::string& path, RendererDevice& rDevice);

	private:
		RendererDevice& rDevice;

		void createVertexBuffers(const std::vector<Vertex>& vertices, Model& out);
		void createIndexBuffers(const std::vector<uint32_t> &indices, Model& out);
		static void createVertexBuffers(const std::vector<Vertex>& vertices, Model& out, RendererDevice& rDevice);
		static void createIndexBuffers(const std::vector<uint32_t>& indices, Model& out, RendererDevice& rDevice);
	};
}