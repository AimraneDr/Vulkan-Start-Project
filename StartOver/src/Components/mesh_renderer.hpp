#pragma once
#include "Renderer/buffer.hpp"
#include "Model/model.hpp"
#include <memory>

namespace Components {
	struct MeshRenderer {
		SO::Model mesh;
	};
}