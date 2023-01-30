#pragma once

#include "Renderer/device.hpp"
#include "Renderer/pipeline.hpp"
#include "GameObject/gameobject.hpp"
#include "Components/camera.hpp"
#include "Renderer/frame_info.hpp"


#include <memory>
#include <vector>

namespace SO {
	class RenderSystem {
	public:

		RenderSystem(RendererDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo);
	private:
		RendererDevice& rDevice;
		std::unique_ptr<Pipeline> gPipeline;
		VkPipelineLayout pipelineLayout;

		void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
		void createPipeline(VkRenderPass renderPass);
	};
}