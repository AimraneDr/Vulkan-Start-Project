#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace SO {
	class GameWindow {
	public:
		GameWindow(int width, int height, const char* name);
		~GameWindow();

		GameWindow(const GameWindow&) = delete;
		GameWindow& operator=(const GameWindow&) = delete;

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		GLFWwindow* getGLFWwindow() const { return window; }
		bool wasWindowResized() { return frameBufferResized; };
		void resetWindowResizedFlag() { frameBufferResized = false; };
		bool shouldClose();
	private:
		GLFWwindow* window;
		int width;
		int height;
		bool frameBufferResized = false;
		const char* name;

		void initWindow();
		static void frameBufferResizedCallBack(GLFWwindow* window, int width, int height);
	};
}