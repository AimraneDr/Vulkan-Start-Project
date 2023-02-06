#pragma once

#include "Renderer/device.hpp"
#include "Window/window.hpp"
#include "Renderer/swapchain.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace SO {
	class GameRenderer {
	public:


		GameRenderer(GameWindow& window, RendererDevice& device);
		~GameRenderer();

		GameRenderer(const GameRenderer&) = delete;
		GameRenderer& operator=(const GameRenderer&) = delete;

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
		float getAspectRatio() const { return swapChain->extentAspectRatio(); }
		bool isFrameInProgress() { return isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get current command buffer when frame not in progress!");
			return commandBuffers[currentFrameIndex];
		}
		VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
		int getFrameIndex() const { 
			assert(isFrameStarted && "Cannot get frame index when frame not in progress!");
			return currentFrameIndex; 
		}
	private:
		GameWindow& gWindow;
		RendererDevice& rDevice;
		std::unique_ptr <SwapChain> swapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted = false;

		void createCommaandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
	};
}