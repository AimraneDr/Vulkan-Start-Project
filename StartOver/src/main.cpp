
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <iostream>

#include "Application/application.hpp"

#include <stdlib.h>
#include <iostream>
#include <stdexcept>


int main() {
	SO::App app{};

	try {
		app.run();
	}
	catch (const std::exception &ex) {
		std::cerr << ex.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}