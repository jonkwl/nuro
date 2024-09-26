#include "shader_builder.h"

std::vector<Shader*> ShaderBuilder::loadAndCompile(std::vector<std::string> paths) {
	std::vector<Shader*> shaders;

	Log::printProcessStart("ShaderBuilder", "Building shaders...");
	Log::printProcessState("ShaderBuilder", "Fetching shaders...");

	std::vector<std::string> shader_paths;
	std::vector<std::string> shader_names;

	for (int i = 0; i < paths.size(); i++) {

		Log::printProcessInfo(paths[i] + ":");

		std::vector<std::string> shaders_in_folder = IOHandler::GetFolders(paths[i]);
		for (int x = 0; x < shaders_in_folder.size(); x++) {

			shader_paths.push_back(paths[i] + "/" + shaders_in_folder[i]);
			shader_names.push_back(shaders_in_folder[i]);
			Log::printProcessInfo("- " + shader_names[x]);

		}
	}

	Log::printProcessState("ShaderBuilder", "Compiling shaders...");
	for (int i = 0; i < shader_paths.size(); i++) {
		std::string vertex_code = IOHandler::ReadFile(shader_paths[i] + "/.vertex");
		std::string fragment_code = IOHandler::ReadFile(shader_paths[i] + "/.fragment");
		const char* vertex_src = vertex_code.c_str();
		const char* fragment_src = fragment_code.c_str();

		bool compiled = false;
		Shader* shader = new Shader(vertex_src, fragment_src, compiled);
		if (compiled) {
			shaders.push_back(shader);
			Log::printProcessInfo("Compiled " + shader_names[i]);
		}
	}

	Log::printProcessDone("ShaderBuilder", "Finished building the shaders");

	return shaders;
}