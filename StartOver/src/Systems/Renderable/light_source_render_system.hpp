#pragma once
#include "Renderer/pipeline.hpp"
#include "Renderer/device.hpp"
#include "ECS/system.hpp"
#include "Renderer/frame_info.hpp"

#include <memory>


class PointLightRenderSystem : public System {
public:
	void init(std::shared_ptr<SO::RendererDevice>& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout);
	void shutDown();

	void update(SO::FrameInfo& frameInfo, SO::GlobalUBO& ubo);
	void render(SO::FrameInfo& frameInfo);
private:
	std::shared_ptr<SO::RendererDevice> rDevice;
	std::unique_ptr<SO::Pipeline> gPipeline;
	VkPipelineLayout pipelineLayout;

	void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
	void createPipeline(VkRenderPass renderPass);
};