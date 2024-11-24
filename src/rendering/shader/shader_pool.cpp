#include "shader_pool.h"

std::vector<Shader*> ShaderPool::shaders;
std::vector<std::string> ShaderPool::identifiers;

void ShaderPool::loadAndCompile(std::vector<std::string> paths) {
	Log::printProcessStart("ShaderPool", "Building shaders...");
	Log::printProcessState("ShaderPool", "Fetching shaders...");

	std::vector<std::string> shader_paths;
	std::vector<std::string> shader_names;

	for (int i = 0; i < paths.size(); i++) {

		Log::printProcessInfo(paths[i] + ":");

		std::vector<std::string> shaders_in_folder = IOHandler::GetFolders(paths[i]);
		for (int x = 0; x < shaders_in_folder.size(); x++) {

			shader_paths.push_back(paths[i] + "/" + shaders_in_folder[x]);
			shader_names.push_back(shaders_in_folder[x]);
			Log::printProcessInfo("- " + shader_names[shader_names.size() - 1]);
		}
	}

	Log::printProcessState("ShaderPool", "Compiling shaders...");
	for (int i = 0; i < shader_paths.size(); i++) {
		std::string vertex_code = IOHandler::ReadFile(shader_paths[i] + "/.vert");
		std::string fragment_code = IOHandler::ReadFile(shader_paths[i] + "/.frag");
		const char* vertex_src = vertex_code.c_str();
		const char* fragment_src = fragment_code.c_str();

		bool compiled = false;
		std::string name = shader_names[i];
		Shader* shader = new Shader(vertex_src, fragment_src, compiled, name);
		if (compiled) {
			shaders.push_back(shader);
			identifiers.push_back(name);
			Log::printProcessInfo("Compiled " + name);
		}
	}

	Log::printProcessDone("ShaderPool", "Finished building the shaders");
}

Shader* ShaderPool::get(std::string name) {
	auto it = std::find(identifiers.begin(), identifiers.end(), name);
	if (it != identifiers.end()) {
		int index = std::distance(identifiers.begin(), it);
		return shaders[index];
	}
	else {
		Log::printError("ShaderPool", "Couldn't find shader " + name);
		return nullptr;
	}
}