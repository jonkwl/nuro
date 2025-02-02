#include "shader_pool.h"

#include <unordered_map>

#include "../core/utils/console.h"
#include "../core/utils/iohandler.h"
#include "../core/rendering/shader/shader.h"

namespace ShaderPool {

	Shader* gEmpty = new Shader("empty");
	std::unordered_map<std::string, Shader*> gShaders;

	void load(const std::vector<std::string>& paths)
	{
		Console::out::processStart("ShaderPool", "Building shaders...");
		Console::out::processState("ShaderPool", "Fetching shaders...");

		std::vector<std::string> shader_paths;
		std::vector<std::string> shader_names;

		for (int32_t i = 0; i < paths.size(); i++)
		{

			Console::out::processInfo(paths[i] + ":");

			std::vector<std::string> shaders_in_folder = IOHandler::getFolders(paths[i]);
			for (int32_t x = 0; x < shaders_in_folder.size(); x++)
			{
				shader_paths.push_back(paths[i] + "/" + shaders_in_folder[x]);
				shader_names.push_back(shaders_in_folder[x]);
				Console::out::processInfo("- " + shader_names[shader_names.size() - 1]);
			}
		}

		Console::out::processState("ShaderPool", "Compiling shaders...");
		for (int32_t i = 0; i < shader_paths.size(); i++)
		{
			std::string vertex_code = IOHandler::readFile(shader_paths[i] + "/.vert");
			std::string fragment_code = IOHandler::readFile(shader_paths[i] + "/.frag");
			const char* vertex_src = vertex_code.c_str();
			const char* fragment_src = fragment_code.c_str();

			std::string name = shader_names[i];
			Shader* shader = new Shader(name);
			bool compiled = shader->compile(vertex_src, fragment_src);
			
			if (compiled)
			{
				gShaders[name] = shader;
				Console::out::processInfo("Compiled " + name);
			}
		}

		Console::out::processDone("ShaderPool", "Finished loading shaders");
	}

	Shader* empty()
	{
		return nullptr;
	}

	Shader* get(const std::string& name)
	{
		if (gShaders.find(name) != gShaders.end()) {
			// Shader available, return shader
			return gShaders[name];
		}
		else {
			// Shader not found, return empty shader
			Console::out::warning("ShaderPool", "Shader '" + name + "' was requested but does not exist!");
			return gEmpty;
		}
	}

}