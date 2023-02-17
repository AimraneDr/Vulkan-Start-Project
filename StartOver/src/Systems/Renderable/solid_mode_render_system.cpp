#include "solid_mode_render_system.hpp"

#include "Components/mesh_renderer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <vector>
#include <stdexcept>

struct PushConstData {
	glm::mat4 modelMatrix{ 1.0f };
};

namespace Systems {

	void SolidModeRenderSystem::init(std::shared_ptr<SO::RendererDevice>& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout)
	{
		rDevice = device;
		createPipelineLayout(descriptorSetLayout);
		createPipeline(renderPass);
	}

	void SolidModeRenderSystem::shutDown() {
		vkDestroyPipelineLayout(rDevice->device(), pipelineLayout, nullptr);
	}


	void SolidModeRenderSystem::createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout) {
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

		if (vkCreatePipelineLayout(rDevice->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void SolidModeRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipelineLayout");

		SO::PipelineConfigInfo configInfo{};
		SO::Pipeline::defaultPipelineConfigInfo(configInfo);
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.renderpass = renderPass;
		configInfo.pipelineLayout = pipelineLayout;

		gPipeline = std::make_unique<SO::Pipeline>(*rDevice, "C:/VStudio/StartOver/StartOver/shaders/out/solidmode.vert.spv", "C:/VStudio/StartOver/StartOver/shaders/out/solidmode.frag.spv", configInfo);
	}

	void SolidModeRenderSystem::bind(VkCommandBuffer commandBuffer, SO::Model& m) {
		VkBuffer buffers[] = { m.vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		if (m.hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, m.indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void SolidModeRenderSystem::draw(VkCommandBuffer commandBuffer, SO::Model& m) {
		if (m.hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m.indices.size()), 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, static_cast<uint32_t>(m.vertices.size()), 1, 0, 0);
		}
	}


	void SolidModeRenderSystem::render(SO::FrameInfo& frameInfo) {
		gPipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

		for (auto& entity : mEntities) {
			auto& objectM = SO::GameObject::Manager.GetComponent<Components::MeshRenderer>(entity);
			auto& objectT = SO::GameObject::Manager.GetComponent<Components::TransformComponent>(entity);
			PushConstData push{};
			push.modelMatrix = objectT.mat4();
			vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstData), &push);
			bind(frameInfo.commandBuffer, objectM.mesh);
			draw(frameInfo.commandBuffer, objectM.mesh);
		}
	}

}