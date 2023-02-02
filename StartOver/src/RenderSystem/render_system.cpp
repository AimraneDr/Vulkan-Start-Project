#include "render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>



namespace SO {

	struct PushConstData {
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
	};


	RenderSystem::RenderSystem(RendererDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout)
		: rDevice{device}
	{
		createPipelineLayout(descriptorSetLayout);
		createPipeline(renderPass);
	}
	RenderSystem::~RenderSystem() {
		vkDestroyPipelineLayout(rDevice.device(), pipelineLayout, nullptr);
	}

	void RenderSystem::createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout) {

		VkPushConstantRange pushConstRange{};
		pushConstRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstRange.offset = 0;
		pushConstRange.size = sizeof(PushConstData);

		std::vector<VkDescriptorSetLayout> layouts{ descriptorSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
		pipelineLayoutInfo.pSetLayouts = layouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstRange;

		if (vkCreatePipelineLayout(rDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void RenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipelineLayout");

		SO::PipelineConfigInfo configInfo{};
		Pipeline::defaultPipelineConfigInfo(configInfo);
		configInfo.renderpass = renderPass;
		configInfo.pipelineLayout = pipelineLayout;

		gPipeline = std::make_unique<Pipeline>(rDevice, "C:/VStudio/StartOver/StartOver/shaders/out/vert.spv", "C:/VStudio/StartOver/StartOver/shaders/out/frag.spv", configInfo);
	}

	void RenderSystem::renderGameObjects(FrameInfo& frameInfo) {
		gPipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

		for (auto& kv : frameInfo.gameObjects) {
			GameObject& object = kv.second;
			if (object.model == nullptr) continue;
			PushConstData push{};
			push.modelMatrix = object.transform.mat4();
			push.normalMatrix = object.transform.normalMat();
			vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstData), &push);
			object.model->bind(frameInfo.commandBuffer);
			object.model->draw(frameInfo.commandBuffer);
		}
	}


}