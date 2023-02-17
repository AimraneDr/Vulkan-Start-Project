#pragma once
#include <string>
#include "Model/model.hpp"

namespace SO {
	static class Loader {
	public:
		//Models
		static void LoadObjFormat(const std::string& path, Model& m);
		static Model LoadObjFormat(const std::string& path);
	};
}