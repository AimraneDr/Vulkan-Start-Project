#pragma once

namespace Components {
	struct PointLightComponent {
		glm::vec3 color{ 1.f, 1.f, 1.f };
		float radius{ 1.0f };
		float intensity{ 1.0f };
	};
}