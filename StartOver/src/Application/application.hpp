#pragma once

#include "Renderer/device.hpp"
#include "Window/window.hpp"
#include "GameObject/gameobject.hpp"
#include "Renderer/renderer.hpp"
#include "Renderer/descriptors.hpp"

#include <memory>
#include <vector>

namespace SO {
	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		App();
		~App();

		App(const App&) = delete;
		App&operator=(const App&) = delete;

		void run();
	private:
		GameWindow gWindow{WIDTH, HEIGHT, "StartOver by 'Aimrane Draou'"};
		RendererDevice rDevice{ gWindow };
		GameRenderer gRenderer{ gWindow, rDevice };

		std::unique_ptr<DescriptorPool> globalPool{};
		GameObject::Map gameObjects;

		void loadGameObjects();
	};
}