#pragma once

#include <vulkan/vulkan.h>

namespace SO {
	struct QueueFamilyIndices {
		uint32_t graphicsFamily;
		uint32_t presentFamily;
		bool presentFamilyHasValue = false;
		bool graphicsFamilyHasValue = false;
		bool isComplete() {
			return graphicsFamilyHasValue && presentFamilyHasValue;
		}
	};
}