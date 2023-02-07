#include "light_source_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

struct PointLightPushConstant {
	glm::vec4 position;
	glm::vec4 color;
	float radius;
};

void PointLightRenderSystem::init(std::shared_ptr<SO::RendererDevice>& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout){
	rDevice = device;
	createPipelineLayout(descriptorSetLayout);
	createPipeline(renderPass);
}
void PointLightRenderSystem::shutDown() {
	vkDestroyPipelineLayout(rDevice->device(), pipelineLayout, nullptr);
}

void PointLightRenderSystem::createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout) {
	//25th
	VkPushConstantRange pushConstRange{};
	pushConstRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstRange.offset = 0;
	pushConstRange.size = sizeof(PointLightPushConstant);

	std::vector<VkDescriptorSetLayout> layouts{ descriptorSetLayout };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
	pipelineLayoutInfo.pSetLayouts = layouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstRange;
	//pipelineLayoutInfo.pushConstantRangeCount = 0;
	//pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(rDevice->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void PointLightRenderSystem::createPipeline(VkRenderPass renderPass) {
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipelineLayout");

	SO::PipelineConfigInfo configInfo{};
	SO::Pipeline::defaultPipelineConfigInfo(configInfo);
	configInfo.renderpass = renderPass;
	configInfo.pipelineLayout = pipelineLayout;
	configInfo.bindingDescriptions.clear();
	configInfo.attributeDescriptions.clear();
	gPipeline = std::make_unique<SO::Pipeline>(*rDevice, "C:/VStudio/StartOver/StartOver/shaders/out/pointlight.vert.spv", "C:/VStudio/StartOver/StartOver/shaders/out/pointlight.frag.spv", configInfo);
}

//25th
void PointLightRenderSystem::update(SO::FrameInfo& frameInfo, SO::GlobalUBO& ubo) {
	auto rotateLight = glm::rotate(glm::mat4(1.f), .75f * frameInfo.frameTime, { 0.f, -1.f, 0.f });

	int lightindex = 0;
	for (auto& e : mEntities) {		
		auto& objT = frameInfo.manager.GetComponent<SO::TransformComponent>(e);
		auto& objPL = frameInfo.manager.GetComponent<Components::PointLightComponent>(e);

		// update light position
		objT.position = glm::vec3(rotateLight * glm::vec4(objT.position, 1.f));

		//paas pointlight to ubo
		ubo.pointlights[lightindex].position = glm::vec4{ objT.position, 1.0f };
		ubo.pointlights[lightindex].color = glm::vec4{ objPL.color, objPL.intensity };
		lightindex += 1;
	}
	ubo.numLights = lightindex;
}

void PointLightRenderSystem::render(SO::FrameInfo& frameInfo) {
	gPipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

	//25th
	for (auto& e : mEntities) {
		auto& objT = frameInfo.manager.GetComponent<SO::TransformComponent>(e);
		auto& objPL = frameInfo.manager.GetComponent<Components::PointLightComponent>(e);

		PointLightPushConstant push{};
		push.position = glm::vec4{ objT.position, 1.0f };
		push.color = glm::vec4{ objPL.color, objPL.intensity };
		push.radius = objPL.radius;
		vkCmdPushConstants(
			frameInfo.commandBuffer,
			pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(PointLightPushConstant),
			&push);

		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}
}


