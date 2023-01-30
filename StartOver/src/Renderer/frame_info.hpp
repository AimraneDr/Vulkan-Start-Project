#pragma once

#include <vulkan/vulkan.h>
#include "Components/camera.hpp"
#include "GameObject/gameobject.hpp"

namespace SO {
//#define MAX_POINT_LIGHTS 10

	//25th
	//struct PointLightInfo {
	//	glm::vec4 position;
	//	glm::vec4 color{};
	//};

	//struct GlobalUBO {
	//	glm::mat4 projection{ 1.0f };
	//	glm::mat4 View{ 1.0f };
	//	glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .2f };//w = intensity
	//	PointLightInfo pointlight[MAX_POINT_LIGHTS];
	//	int numLights;
	//};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera camera;
		VkDescriptorSet globalDescriptorSet;
		GameObject::Map& gameObjects;
	};
}