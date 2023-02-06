#pragma once

#include "ECS/ecs_manager.hpp"
#include "Renderer/device.hpp"
#include "Window/window.hpp"
#include "GameObject/gameobject.hpp"
#include "Renderer/renderer.hpp"
#include "Renderer/descriptors.hpp"

#include <memory>

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
		ECSManager manager{};
		GameWindow gWindow{WIDTH, HEIGHT, "StartOver by 'Aimrane Draou'"};
		//RendererDevice rDevice{ gWindow };
		std::shared_ptr<RendererDevice> rDevice = std::make_shared<RendererDevice>(gWindow);
		GameRenderer gRenderer{ gWindow, *rDevice};
		GameModel modeller{ *rDevice };
		std::unique_ptr<DescriptorPool> globalPool{};
		GameObject::Map gameObjects;

		void loadGameObjects();
	};
}