#include "camera_system.hpp"

#include <GameObject/gameobject.hpp>


void CameraSystem::update(float dt, ECSManager& manager, GLFWwindow* window) {
	currentCameraId = *mEntities.begin();
	glm::vec3 rotation{ 0 };
	glm::vec3 position{ 0 };

	auto& objTransfomr = manager.GetComponent<SO::TransformComponent>(currentCameraId);
	auto& camera = manager.GetComponent<Components::CameraComponent>(currentCameraId);
	//rotaion actions
	if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS)rotation.y += 1.0f;
	if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS)rotation.y -= 1.0f;
	if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS)rotation.x += 1.0f;
	if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS)rotation.x -= 1.0f;

	if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon()) {
		objTransfomr.rotation += turnSpeed * dt * glm::normalize(rotation);
	}

	objTransfomr.rotation.x = glm::clamp(objTransfomr.rotation.x, -1.5f, 1.5f);
	objTransfomr.rotation.y = glm::mod(objTransfomr.rotation.y, glm::two_pi<float>());


	//translation actions
	float yaw = objTransfomr.rotation.y;
	const glm::vec3 forwardDir{ sin(yaw), 0.0f, cos(yaw) };
	const glm::vec3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x };
	const glm::vec3 upDir{ .0f, -1.0f, .0f };

	glm::vec3 moveDir{ 0.f };

	if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)moveDir += forwardDir;
	if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)moveDir -= forwardDir;
	if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)moveDir += rightDir;
	if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)moveDir -= rightDir;
	if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS)moveDir += upDir;
	if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS)moveDir -= upDir;

	if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
		objTransfomr.position += moveSpeed * dt * glm::normalize(moveDir);
	}

	camera.setViewYXZ(objTransfomr.position, objTransfomr.rotation);

	camera.setPerspectiveProjection(glm::radians(50.0f));
}
