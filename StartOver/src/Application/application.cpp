#include "application.hpp"

#include "Components/camera.hpp"
#include "Components/mesh_renderer.hpp"
#include "Systems/Physics/physics.hpp"
#include "Systems/Renderable/renderable_system.hpp"
#include "Systems/Renderable/light_source_render_system.hpp"
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
	std::shared_ptr<CameraSystem> cameraS;
	std::shared_ptr<PhysicsSystem> physicsS;
	std::shared_ptr<RenderablesSystem> renderablesS;
	std::shared_ptr<PointLightRenderSystem> pointLightRenderS;

	App::App() {
		manager.Init();
		manager.RegisterComponent<Components::CameraComponent>();
		manager.RegisterComponent<TransformComponent>();
		manager.RegisterComponent<MovementComponent>();
		manager.RegisterComponent<Components::MeshRenderer>();
		manager.RegisterComponent<Components::PointLightComponent>();

		cameraS = manager.RegisterSystem<CameraSystem>();
		{
			Signature signature, ro_signature;
			signature.set(manager.GetComponentType<Components::CameraComponent>());
			signature.set(manager.GetComponentType<TransformComponent>());
			manager.SetSystemSignature<CameraSystem>(signature);
		}
		physicsS = manager.RegisterSystem<PhysicsSystem>();
		{
			Signature signature, ro_signature;
			signature.set(manager.GetComponentType<TransformComponent>());
			signature.set(manager.GetComponentType<MovementComponent>());
			manager.SetSystemSignature<PhysicsSystem>(signature);
		}
		renderablesS = manager.RegisterSystem<RenderablesSystem>();
		{
			Signature signature;
			signature.set(manager.GetComponentType<Components::MeshRenderer>());
			manager.SetSystemSignature<RenderablesSystem>(signature);
		}
		pointLightRenderS = manager.RegisterSystem<PointLightRenderSystem>();
		{
			Signature signature;
			signature.set(manager.GetComponentType<TransformComponent>());
			signature.set(manager.GetComponentType<Components::PointLightComponent>());
			manager.SetSystemSignature<PointLightRenderSystem>(signature);
		}


		globalPool = DescriptorPool::Builder(*rDevice)
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}
	App::~App() {
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
		pointLightRenderS->init(rDevice, gRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());

		
		Entity camEID = manager.CreateEntity();
		manager.AddComponent(camEID, TransformComponent{
			{.0f,.0f,-4.f},
			{1.f,1.f,1.f},
			{.0f,.0f,.0f}
			});
		manager.AddComponent(camEID, Components::CameraComponent{});

		Components::CameraComponent& camera = manager.GetComponent<Components::CameraComponent>(camEID);
		camera.setViewDirection(glm::vec3{ 0.f }, glm::vec3{ .0f, 0.f,1.f });
		camera.aspect = gRenderer.getAspectRatio();

		TransformComponent camTransfomr = manager.GetComponent<TransformComponent>(camEID);

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!gWindow.shouldClose()) {
			glfwPollEvents();


			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			frameTime = glm::min(frameTime, MAX_DELTA_TIME);

			cameraS->update(frameTime, manager,gWindow.getGLFWwindow());


			if (VkCommandBuffer commandBuffer = gRenderer.beginFrame()) {
				int frameIndex = gRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					manager
				};
				//update
				GlobalUBO ubo{};
				ubo.projection = camera.getProjectionMat();
				ubo.view = camera.getViewMat();
				ubo.inverseView = camera.getInverseViewMat();
				//Update Systems
				physicsS->Update(frameTime, manager);
				pointLightRenderS->update(frameInfo, ubo);

				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//render
				gRenderer.beginSwapChainRenderPass(commandBuffer);
				renderablesS->render(frameInfo);
				pointLightRenderS->render(frameInfo);
				gRenderer.endSwapChainRenderPass(commandBuffer);
				gRenderer.endFrame();
			}
		}


		vkDeviceWaitIdle(rDevice->device());
		renderablesS->shutDown();
		pointLightRenderS->shutDown();
	}

    // temporary helper function, creates a 1x1x1 cube centered at offset
    std::unique_ptr<GameModel> createCubeModel(RendererDevice& device, glm::vec3 offset) {
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

		//Use without indices
		//builder.vertices = {

		//	// top face (white)
		//	{a, {.9f, .9f, .9f}},
		//	{c, {.9f, .9f, .9f}},
		//	{b, {.9f, .9f, .9f}},
		//	{a, {.9f, .9f, .9f}},
		//	{d, {.9f, .9f, .9f}},
		//	{c, {.9f, .9f, .9f}},

		//	// right face (yellow)
		//	{a, {.8f, .4f, .1f}},
		//	{f, {.8f, .4f, .1f}},
		//	{e, {.8f, .4f, .1f}},
		//	{a, {.8f, .4f, .1f}},
		//	{b, {.8f, .4f, .1f}},
		//	{f, {.8f, .4f, .1f}},

		//	// front face (orange)
		//	{b, {.9f, .6f, .1f}},
		//	{c, {.9f, .6f, .1f}},
		//	{f, {.9f, .6f, .1f}},
		//	{c, {.9f, .6f, .1f}},
		//	{g, {.9f, .6f, .1f}},
		//	{f, {.9f, .6f, .1f}},

		//	// left face (red)
		//	{c, {.8f, .1f, .1f}},
		//	{d, {.8f, .1f, .1f}},
		//	{g, {.8f, .1f, .1f}},
		//	{d, {.8f, .1f, .1f}},
		//	{h, {.8f, .1f, .1f}},
		//	{g, {.8f, .1f, .1f}},

		//	// bottom face (blue)
		//	{f, {.1f, .1f, .8f}},
		//	{h, {.1f, .1f, .8f}},
		//	{e, {.1f, .1f, .8f}},
		//	{f, {.1f, .1f, .8f}},
		//	{g, {.1f, .1f, .8f}},
		//	{h, {.1f, .1f, .8f}},

		//	// back face (green)
		//	{a, {.1f, .8f, .1f}},
		//	{h, {.1f, .8f, .1f}},
		//	{d, {.1f, .8f, .1f}},
		//	{a, {.1f, .8f, .1f}},
		//	{e, {.1f, .8f, .1f}},
		//	{h, {.1f, .8f, .1f}}

		//};
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
        return std::make_unique<GameModel>(device);
    }

	void App::loadGameObjects() {

		Entity e = manager.CreateEntity();
		GameObject cube = GameObject::createGameObject();
		cube.eid = e;
		manager.AddComponent(
			e,
			Components::MeshRenderer{
				modeller.loadModel("C:\\VStudio\\StartOver\\StartOver\\assets\\models\\colored_cube.obj")
			}
		);
		manager.AddComponent(
			e,
			TransformComponent{
				glm::vec3{1.5f, .0f, .0f},
				glm::vec3{.5f, .5f, .5f},
				glm::vec3{.0f, .0f, .0f}
			}
		);
		manager.AddComponent(
			e,
			MovementComponent{
				glm::vec3{.0f, -2.f, .0f},
			}
		);


		e = manager.CreateEntity();
		GameObject cube1 = GameObject::createGameObject();
		cube1.eid = e;
		manager.AddComponent(
			e,
			Components::MeshRenderer{
				modeller.loadModel("C:\\VStudio\\StartOver\\StartOver\\assets\\models\\smooth_vase.obj")
			}
		);
		manager.AddComponent(
			e,
			TransformComponent{
				glm::vec3{.5f, .5f, .5f},
				glm::vec3{2.f, 2.f, 2.f},
				glm::vec3{.0f, .0f, .0f}
			}
		);


		e = manager.CreateEntity();
		GameObject cube2 = GameObject::createGameObject();
		cube2.eid = e;
		manager.AddComponent(
			e,
			Components::MeshRenderer{
				modeller.loadModel("C:\\VStudio\\StartOver\\StartOver\\assets\\models\\flat_vase.obj")
			}
		);
		manager.AddComponent(
			e,
			TransformComponent{
				glm::vec3{-1.5f, .5f, 0.f},
				glm::vec3{2.2f,3.f,1.5f},
				glm::vec3{.0f, .0f, .0f}
			}
		);

		e = manager.CreateEntity();
		GameObject cube3 = GameObject::createGameObject();
		cube3.eid = e;
		manager.AddComponent(
			e,
			Components::MeshRenderer{
				modeller.loadModel("C:\\VStudio\\StartOver\\StartOver\\assets\\models\\quad.obj")
			}
		);
		manager.AddComponent(
			e,
			TransformComponent{
				glm::vec3{.0f, .5f, .0f},
				glm::vec3{10.f, 10.f, 10.f},
				glm::vec3{.0f, .0f, .0f}
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
			e = manager.CreateEntity();
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f });
			manager.AddComponent(
				e,
				TransformComponent{
					glm::vec3(rotateLight * glm::vec4(-3.f, -1.5f, -3.f, 1.f)),
					glm::vec3{1.f, 1.f, 1.f},
					glm::vec3{.0f, .0f, .0f}
				}
			);
			manager.AddComponent(
				e,
				Components::PointLightComponent{
					lightColors[i],
					.25f,
					a
				}
			);
		}


	}
}