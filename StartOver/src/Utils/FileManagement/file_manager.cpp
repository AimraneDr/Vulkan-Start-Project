#include "file_manager.h"

#include <fstream>

std::vector<char> readFile(const std::string& path) {
	std::ifstream file{ path, std::ios::ate | std::ios::binary };

	if (!file.is_open()) {
		throw std::runtime_error("Faild to Open file :" + path);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}