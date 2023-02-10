#pragma once
#include "Renderer/pipeline.hpp"
#include "Renderer/device.hpp"
#include "ECS/system.hpp"
#include "Renderer/frame_info.hpp"
#include "Model/model.hpp"

#include <memory>

namespace Systems {

	class SolidModeRenderSystem : public System {
	public:
		void init(std::shared_ptr<SO::RendererDevice>& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout);
		void shutDown();
		void render(SO::FrameInfo& frameInfo);
	private:
		std::shared_ptr<SO::RendererDevice> rDevice;
		std::unique_ptr<SO::Pipeline> gPipeline;
		VkPipelineLayout pipelineLayout;

		void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
		void createPipeline(VkRenderPass renderPass);
		void bind(VkCommandBuffer commandBuffer, SO::Model& m);
		void draw(VkCommandBuffer commandBuffer, SO::Model& m);
	};
}