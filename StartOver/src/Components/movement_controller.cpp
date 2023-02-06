#include "movement_controller.hpp"

namespace SO {
	void MovementController::moveInPlaneXZ(GLFWwindow* window, float deltaTime) {
		glm::vec3 rotation{ 0 };
		glm::vec3 position{ 0 };
		//rotaion actions
		if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS)rotation.y += 1.0f;
		if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS)rotation.y -= 1.0f;
		if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS)rotation.x += 1.0f;
		if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS)rotation.x -= 1.0f;

		if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon()) {
			objTransfomr.rotation += turnSpeed * deltaTime * glm::normalize(rotation);
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
			objTransfomr.position += moveSpeed * deltaTime * glm::normalize(moveDir);
		}

	}

}