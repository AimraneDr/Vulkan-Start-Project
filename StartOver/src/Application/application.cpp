#include "application.hpp"

#include "RenderSystem/render_system.hpp"
#include "RenderSystem/pointlight_render_system.hpp"
#include "Components/camera.hpp"
#include "Components/movement_controller.hpp"

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


	App::App() {
		globalPool = DescriptorPool::Builder(rDevice)
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
				rDevice,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = DescriptorSetLayout::Builder(rDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		RenderSystem simpleRenderSystem{ rDevice, gRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PointLightRenderSystem pointLightRenderSystem{rDevice, gRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
		Camera camera{};
		camera.setViewDirection(glm::vec3{ 0.f }, glm::vec3{ .0f, 0.f,1.f });
		
		GameObject camObj = GameObject::createGameObject();
		camObj.transform.position.z = -4.f;
		MovementController controller{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!gWindow.shouldClose()) {
			glfwPollEvents();


			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			frameTime = glm::min(frameTime, MAX_DELTA_TIME);

			controller.moveInPlaneXZ(gWindow.getGLFWwindow(), frameTime, camObj);
			camera.setViewYXZ(camObj.transform.position, camObj.transform.rotation);

			float aspect = gRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, .1f, 100.f);

			if (VkCommandBuffer commandBuffer = gRenderer.beginFrame()) {
				int frameIndex = gRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};
				//update
				GlobalUBO ubo{};
				ubo.projection = camera.getProjectionMat();
				ubo.View = camera.getViewMat();
				pointLightRenderSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//render
				gRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightRenderSystem.render(frameInfo);
				gRenderer.endSwapChainRenderPass(commandBuffer);
				gRenderer.endFrame();
			}
		}


		vkDeviceWaitIdle(rDevice.device());
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
		std::vector<GameModel::Vertex> vertices = {
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
        return std::make_unique<GameModel>(device, vertices, indices);
    }

	void App::loadGameObjects() {

		//std::shared_ptr<GameModel> model = createCubeModel(rDevice, { .0f,.0f,.0f });
		std::shared_ptr<GameModel> model = GameModel::loadModelFromFile(rDevice, "C:\\VStudio\\StartOver\\StartOver\\assets\\models\\colored_cube.obj");
		GameObject cube = GameObject::createGameObject();
        cube.model = model;
        cube.transform.position = { .5f, .0f, 0.f };
        cube.transform.scale = { .5f,.5f,.5f };

		std::shared_ptr<GameModel> model1 = GameModel::loadModelFromFile(rDevice, "C:\\VStudio\\StartOver\\StartOver\\assets\\models\\smooth_vase.obj");
		GameObject cube1 = GameObject::createGameObject();
		cube1.model = model1;
		cube1.transform.position = { -.5f, .5f, 0.f };
		cube1.transform.scale = { 2.f,2.f,2.f };

		std::shared_ptr<GameModel> model2 = GameModel::loadModelFromFile(rDevice, "C:\\VStudio\\StartOver\\StartOver\\assets\\models\\flat_vase.obj");
		GameObject cube2 = GameObject::createGameObject();
		cube2.model = model2;
		cube2.transform.position = { -1.5f, .5f, 0.f };
		cube2.transform.scale = { 2.2f,3.f,1.5f };

		std::shared_ptr<GameModel> model3 = GameModel::loadModelFromFile(rDevice, "C:\\VStudio\\StartOver\\StartOver\\assets\\models\\quad.obj");
		GameObject cube3 = GameObject::createGameObject();
		cube3.model = model3;
		cube3.transform.position = { 0.f, .5f, 0.f };
		cube3.transform.scale = { 10.f,10.f,10.f };

		
		gameObjects.emplace(cube.getID(), std::move(cube));
		gameObjects.emplace(cube1.getID(), std::move(cube1));
		gameObjects.emplace(cube2.getID(), std::move(cube2));
		gameObjects.emplace(cube3.getID(), std::move(cube3));


		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}  //
		};

		for (int i = 0; i < lightColors.size(); i++) {
			auto pointLight = GameObject::createPointLightGameObject(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f });
			pointLight.transform.position = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			gameObjects.emplace(pointLight.getID(), std::move(pointLight));
		}
		
	}
}