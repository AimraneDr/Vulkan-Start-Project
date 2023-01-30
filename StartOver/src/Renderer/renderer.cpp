#include "renderer.hpp"


#include <stdexcept>
#include <array>

namespace SO {

	GameRenderer::GameRenderer(GameWindow& window, RendererDevice& device)
		:gWindow{ window }, rDevice{ device }
	{
		recreateSwapChain();
		createCommaandBuffers();
	}
	GameRenderer::~GameRenderer() {
		freeCommandBuffers();
	}

	void GameRenderer::recreateSwapChain() {
		VkExtent2D extent = gWindow.getExtent();
		//Experement with the size limits
		while (extent.width == 0 || extent.height == 0) {
			extent = gWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(rDevice.device());
		if (swapChain == nullptr) {
			swapChain = std::make_unique<SwapChain>(rDevice, extent);
		}
		else {
			std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain);
			swapChain = std::make_unique<SwapChain>(rDevice, extent, std::move(swapChain));
			
			if (!oldSwapChain->compareSwapFormats(*swapChain.get())) {
				throw std::runtime_error("SwapShain image (or depth) format has changed!");
			}
		}
	}

	void GameRenderer::createCommaandBuffers() {
		commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = rDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(rDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Allocate Command Buffers");
		}
	}

	void GameRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(rDevice.device(), rDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer GameRenderer::beginFrame() {
		assert(!isFrameStarted && "Cannot call beginFrame while Frame already in progress!");

		VkResult result = swapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to Acquire SawpChain Image");
		}
		isFrameStarted = true;
		
		VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording Command Buffer");
		}
		return commandBuffer;
	}

	void GameRenderer::endFrame() {
		assert(isFrameStarted && "Cannot call endFrame while Frame Not in progress!");

		VkCommandBuffer commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to end recording command buffer");
		}

		VkResult result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || gWindow.wasWindowResized()) {
			gWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to Present SawpChain Image");
		}
		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void GameRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cannot call beginSwapChainRenderPass while Frame already in progress!");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin render pass on command buffer from a different frame!");

		VkRenderPassBeginInfo renderPassBegininfo{};
		renderPassBegininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBegininfo.renderPass = swapChain->getRenderPass();
		renderPassBegininfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);
		renderPassBegininfo.renderArea.offset = { 0,0 };
		renderPassBegininfo.renderArea.extent = swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.015f, 0.015f, 0.015f, 0.0f };
		clearValues[1].depthStencil = { 1.0f,0 };

		renderPassBegininfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBegininfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBegininfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChain->getSwapChainExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void GameRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cannot call endSwapChainRenderPass while Frame Not in progress!");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end render pass on command buffer from a different frame!");

		vkCmdEndRenderPass(commandBuffer);
	}

}