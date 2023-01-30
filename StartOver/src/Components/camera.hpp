#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace SO{
	class Camera {
	public:
		Camera();
		~Camera();

		void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
		void setPerspectiveProjection(float fovy, float aspect, float near, float far);

		void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ .0f,-1.0f,.0f });
		void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ .0f,-1.0f,.0f });
		void setViewYXZ(glm::vec3 position, glm::vec3 rotation);


		const glm::mat4 getProjectionMat()const { return projectionMat; }
		const glm::mat4 getViewMat()const { return viewMat; }
	private:
		glm::mat4 projectionMat{ 1.0f };
		glm::mat4 viewMat{ 1.0f };
	};
}