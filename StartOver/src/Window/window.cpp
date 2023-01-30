#include "window.hpp"

#include <stdexcept>

namespace SO {

	GameWindow::GameWindow(int width, int height, const char* name) :width{ width }, height{ height }, name{ name } {
		initWindow();
	}

	GameWindow::~GameWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void GameWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = glfwCreateWindow(width, height, name, nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizedCallBack);
	}

	void GameWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Faild to create Window Surface !");
		}
	}
	

	bool GameWindow::shouldClose() {
		return glfwWindowShouldClose(window);
	}

	void GameWindow::frameBufferResizedCallBack(GLFWwindow* window, int width, int height) {
		GameWindow* win = reinterpret_cast<GameWindow*>(glfwGetWindowUserPointer(window));
		win->frameBufferResized = true;
		win->width = width;
		win->height = height;
	}
}