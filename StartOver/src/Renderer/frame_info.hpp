#pragma once

#include <vulkan/vulkan.h>
#include "Components/camera.hpp"
#include "GameObject/gameobject.hpp"
#include "ECS/ecs_manager.hpp"


namespace SO {
#define MAX_POINT_LIGHTS 10

	//25th
	struct PointLight {
		glm::vec4 position;
		glm::vec4 color{};
	};


	//25th
	struct GlobalUBO {
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::mat4 inverseView{ 1.0f };
		PointLight pointlights[MAX_POINT_LIGHTS];
		glm::vec4 directionalLight{ 1.f, -3.f, -1.f, 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };//w = intensity
		int numLights;
		float ambient{ 0.05 };
	};
	//struct GlobalUBO {
	//	glm::mat4 projection{ 1.f };
	//	glm::mat4 view{ 1.f };
	//	glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };  // w is intensity
	//	glm::vec3 lightPosition{ -1.f };
	//	alignas(16) glm::vec4 lightColor{ 1.f };  // w is light intensity
	//};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Components::CameraComponent& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}