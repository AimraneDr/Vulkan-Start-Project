#pragma once
#include "GLFW/glfw3.h"
#include "Window/window.hpp"
#include "GameObject/gameobject.hpp"

namespace SO {
	class MovementController {
	public:
		struct KeyMappings {
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveUp = GLFW_KEY_Q;
			int moveDown = GLFW_KEY_E;
			int lookLeft = GLFW_KEY_LEFT;
			int lookRight = GLFW_KEY_RIGHT;
			int lookUp = GLFW_KEY_UP;
			int lookDown = GLFW_KEY_DOWN;
		};

		KeyMappings keys{};
		float moveSpeed{3.f}, turnSpeed{1.5f};

		MovementController(TransformComponent& target) : objTransfomr{ target } {};
		~MovementController() = default;

		void moveInPlaneXZ(GLFWwindow* window, float deltaTime);
	private:
		TransformComponent& objTransfomr;
	};
}