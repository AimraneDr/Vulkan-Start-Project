#pragma once

#include "Renderer/device.hpp"
#include "Renderer/pipeline.hpp"
#include "GameObject/gameobject.hpp"
#include "Components/camera.hpp"
#include "Renderer/frame_info.hpp"


#include <memory>
#include <vector>

namespace SO {

	class PointLightRenderSystem {
	public:

		PointLightRenderSystem(RendererDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout);
		~PointLightRenderSystem();

		PointLightRenderSystem(const PointLightRenderSystem&) = delete;
		PointLightRenderSystem& operator=(const PointLightRenderSystem&) = delete;

		//void update(FrameInfo& frameInfo, GlobalUBO ubo);
		void render(FrameInfo& frameInfo);
	private:
		RendererDevice& rDevice;
		std::unique_ptr<Pipeline> gPipeline;
		VkPipelineLayout pipelineLayout;

		void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
		void createPipeline(VkRenderPass renderPass);
	};
}