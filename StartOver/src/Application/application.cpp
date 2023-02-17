#include "application.hpp"

#include "Components/components.h"
#include "Resources/resources_loader.hpp"
#include "Model/model.hpp"
#include "GameObject/gameobject.hpp"
#include "Systems/Physics/physics.hpp"
#include "Systems/Renderable/renderable_system.hpp"
#include "Systems/Renderable/wireframe_render_system.hpp"
#include "Systems/Renderable/solid_mode_render_system.hpp"
#include "Systems/Renderable/light_source_render_system.hpp"
#include "Systems/Renderable/debug_collision_render_system.hpp"
#include "Systems/Camera/camera_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <chrono>
#include <stdexcept>
#include <array>
#include <numeric>



namespace SO {
	const float MAX_DELTA_TIME{ 0.01 };
	ECSManager& manager{ GameObject::Manager };

	std::shared_ptr<Systems::CameraSystem> cameraS;
	std::shared_ptr<Systems::PhysicsSystem> physicsS;
	std::shared_ptr<Systems::RenderablesSystem> renderablesS;
	std::shared_ptr<Systems::WireframeRenderSystem> wireframeRenderS;
	std::shared_ptr<Systems::SolidModeRenderSystem> SolidModeRenderS;
	std::shared_ptr<Systems::PointLightRenderSystem> pointLightRenderS;
	std::shared_ptr<Systems::DebugCollisionRenderSystem> debugCollRenderS;

	bool
		enableWireFrame = false,
		enableSolidMode = false,
		enableRenderView = true,
		enableDebugCollView = false;

	App::App() {
		manager.Init();
		manager.RegisterComponent<Components::CameraComponent>();
		manager.RegisterComponent<Components::TransformComponent>();
		manager.RegisterComponent<MovementComponent>();
		manager.RegisterComponent<Components::MeshRenderer>();
		manager.RegisterComponent<Components::PointLightComponent>();
		manager.RegisterComponent<Components::BoxCollider>();

		cameraS = manager.RegisterSystem<Systems::CameraSystem>();
		{
			Signature signature, ro_signature;
			signature.set(manager.GetComponentType<Components::CameraComponent>());
			signature.set(manager.GetComponentType<Components::TransformComponent>());
			manager.SetSystemSignature<Systems::CameraSystem>(signature);
		}
		physicsS = manager.RegisterSystem<Systems::PhysicsSystem>();
		{
			Signature signature, ro_signature;
			signature.set(manager.GetComponentType<Components::TransformComponent>());
			signature.set(manager.GetComponentType<MovementComponent>());
			signature.set(manager.GetComponentType<Components::BoxCollider>());
			manager.SetSystemSignature<Systems::PhysicsSystem>(signature);
		}
		renderablesS = manager.RegisterSystem<Systems::RenderablesSystem>();
		{
			Signature signature;
			signature.set(manager.GetComponentType<Components::MeshRenderer>());
			manager.SetSystemSignature<Systems::RenderablesSystem>(signature);
		}
		wireframeRenderS = manager.RegisterSystem<Systems::WireframeRenderSystem>();
		{
			Signature signature;
			signature.set(manager.GetComponentType<Components::MeshRenderer>());
			manager.SetSystemSignature<Systems::WireframeRenderSystem>(signature);
		}
		SolidModeRenderS = manager.RegisterSystem<Systems::SolidModeRenderSystem>();
		{
			Signature signature;
			signature.set(manager.GetComponentType<Components::MeshRenderer>());
			manager.SetSystemSignature<Systems::SolidModeRenderSystem>(signature);
		}
		pointLightRenderS = manager.RegisterSystem<Systems::PointLightRenderSystem>();
		{
			Signature signature;
			signature.set(manager.GetComponentType<Components::TransformComponent>());
			signature.set(manager.GetComponentType<Components::PointLightComponent>());
			manager.SetSystemSignature<Systems::PointLightRenderSystem>(signature);
		}
		debugCollRenderS = manager.RegisterSystem<Systems::DebugCollisionRenderSystem>();
		{
			Signature signature;
			signature.set(manager.GetComponentType<Components::TransformComponent>());
			signature.set(manager.GetComponentType<Components::BoxCollider>());
			manager.SetSystemSignature<Systems::DebugCollisionRenderSystem>(signature);
		}


		globalPool = DescriptorPool::Builder(*rDevice)
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}
	App::~App() {
	}

	void processInputs(GLFWwindow* window) {
		
		if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && enableWireFrame) {
			//disable if enabled
			enableWireFrame = false;
			if (!enableSolidMode && !enableDebugCollView)	enableRenderView = true;
		}else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
			//enable
			enableWireFrame = true;
			enableRenderView = false;
		}

		if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && enableSolidMode) {
			//disable if enabled
			enableSolidMode = false;
			if (!enableWireFrame && !enableDebugCollView)	enableRenderView = true;
		}else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
			//enable
			enableRenderView = false;
			enableSolidMode = true;
		}

		if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && enableDebugCollView) {
			//disable if enabled
			enableDebugCollView = false;
			if (!enableWireFrame && !enableSolidMode)	enableRenderView = true;
		}else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
			//enable
			enableDebugCollView = true;
		}
		
		if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
			enableWireFrame = false;
			enableSolidMode = false;
			enableRenderView = true;
		}



	}

	void App::run() {
		//Uniform Buffer
		std::vector<std::unique_ptr<RendererBuffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<RendererBuffer>(
				*rDevice,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = DescriptorSetLayout::Builder(*rDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}
		renderablesS->init(rDevice, gRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
		wireframeRenderS->init(rDevice, gRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
		SolidModeRenderS->init(rDevice, gRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
		pointLightRenderS->init(rDevice, gRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
		debugCollRenderS->init(rDevice, gRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());

		GameObject camObj = GameObject::createGameObject(
			Components::TransformComponent{
				{.0f,-5.f,-15.f}
			}
		);
		camObj.AddComponent(Components::CameraComponent{});

		Components::CameraComponent& camera = camObj.GetComponent<Components::CameraComponent>();
		camera.setViewDirection(glm::vec3{ 0.f }, glm::vec3{ .0f, 0.f,1.f });


		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!gWindow.shouldClose()) {
			glfwPollEvents();
			processInputs(gWindow.getGLFWwindow());

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			frameTime = glm::min(frameTime, MAX_DELTA_TIME);

			camera.aspect = gRenderer.getAspectRatio();
			cameraS->update(frameTime, gWindow.getGLFWwindow());


			if (VkCommandBuffer commandBuffer = gRenderer.beginFrame()) {
				int frameIndex = gRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex]
				};
				//update
				GlobalUBO ubo{};
				ubo.projection = camera.getProjectionMat();
				ubo.view = camera.getViewMat();
				ubo.inverseView = camera.getInverseViewMat();
				//Update Systems
				physicsS->Update(frameTime);
				pointLightRenderS->update(frameInfo, ubo);

				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//render
				gRenderer.beginSwapChainRenderPass(commandBuffer);
				if (enableWireFrame)	wireframeRenderS->render(frameInfo);
				if (enableSolidMode)	SolidModeRenderS->render(frameInfo);
				if (enableDebugCollView)debugCollRenderS->render(frameInfo);
				if (enableRenderView) {
					pointLightRenderS->render(frameInfo);
					renderablesS->render(frameInfo);
				}

				gRenderer.endSwapChainRenderPass(commandBuffer);
				gRenderer.endFrame();
			}
		}


		vkDeviceWaitIdle(rDevice->device());
		renderablesS->shutDown();
		wireframeRenderS->shutDown();
		SolidModeRenderS->shutDown();
		pointLightRenderS->shutDown();
		debugCollRenderS->shutDown();
	}

    // temporary helper function, creates a 1x1x1 cube centered at offset
    std::unique_ptr<Model> createCubeModel(RendererDevice& device, glm::vec3 offset) {
		glm::vec3 a{ .5f, -.5f, .5f };
		glm::vec3 b{ .5f, -.5f, -.5f };
		glm::vec3 c{ -.5f, -.5f, -.5f };
		glm::vec3 d{ -.5f, -.5f, .5f };
		glm::vec3 e{ .5f, .5f, .5f };
		glm::vec3 f{ .5f, .5f, -.5f };
		glm::vec3 g{ -.5f, .5f, -.5f };
		glm::vec3 h{ -.5f, .5f, .5f };

		glm::vec3 white{ .9f, .9f, .9f };
		glm::vec3 yellow{ .8f, .4f, .1f };
		glm::vec3 orange{ .9f, .6f, .1f };
		glm::vec3 red{ .8f, .1f, .1f };
		glm::vec3 blue{ .1f, .1f, .8f };
		glm::vec3 green{ .1f, .8f, .1f };
		glm::vec3 brown{ .39f, .26f, .12f };
		glm::vec3 violet{ .46f, .22f, .69f };

		
		std::vector<Vertex> vertices = {
			{a, white},
			{b, yellow},
			{c, orange},
			{d, red},
			{e, blue},
			{f, green},
			{g, brown},
			{h, violet}
		};
		std::vector<uint32_t> indices = { 0,2,1, 0,3,2, 0,5,4, 0,1,5, 1,2,5, 2,6,5, 2,3,6, 3,7,6, 5,7,4, 5,6,7, 0,7,3, 0,4,7 };
        for (auto& v : vertices) {
            v.pos += offset;
        }
		std::unique_ptr<Model> hh = std::make_unique<Model>();

        return nullptr;
    }

	void App::loadGameObjects() {
		auto trans = Components::TransformComponent{
			glm::vec3{0.5f, -5.5f, .5f},
			glm::vec3{1.f, 1.f, 1.f},
			glm::vec3{.0f, .0f, .0f}
		};

		GameObject cube = GameObject::createGameObject(trans);
		cube.AddComponent(
			Components::MeshRenderer{
				//Loader::LoadObjFormat("C:\\VStudio\\StartOver\\StartOver\\assets\\models\\colored_cube.obj")
				Loader::LoadObjFormat("C:\\VStudio\\StartOver\\StartOver\\assets\\models\\spher.obj")
			}
		);
		cube.AddComponent(
			MovementComponent{
				glm::vec3{.0f, -10.f, 0.f}
			}
		);
		cube.AddComponent(
			Components::BoxCollider(
				cube.transform.position,
				glm::vec3{ 1.f, 1.f, 1.f }
			)
		);


		//ground
		trans.position = { .0f, .0f, .0f };
		trans.scale = { 204.f, 1.f, 204.f };
		GameObject cube3 = GameObject::createGameObject(trans);
		cube3.AddComponent(
			Components::MeshRenderer{
				Loader::LoadObjFormat("C:\\VStudio\\StartOver\\StartOver\\assets\\models\\quad.obj")
			}
		);

		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}  //
		};
		float a = 0;
		for (int i = 0; i < lightColors.size(); i++) {
			a = i + 1;
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f });

			GameObject lightObj = GameObject::createGameObject(
				Components::TransformComponent{
						glm::vec3(rotateLight * glm::vec4(-3.f, -1.5f, -3.f, 1.f)),
						glm::vec3{1.f, 1.f, 1.f},
						glm::vec3{.0f, .0f, .0f}
				}
			);
			lightObj.AddComponent(
				Components::PointLightComponent{
					lightColors[i],
					.25f,
					a
				}
			);
		}


	}
}