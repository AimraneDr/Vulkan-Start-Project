#pragma once
#include "ECS/ecs_manager.hpp"

#include "Components/camera.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class CameraSystem : public System {
public :
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
	float moveSpeed{ 3.f }, turnSpeed{ 1.5f };

	void update(float dt, ECSManager& manager, GLFWwindow* window);
private:
	Entity currentCameraId;
};