#include "shader_pool.h"

#include "../core/utils/console.h"
#include "../core/utils/iohandler.h"
#include "../core/rendering/shader/shader.h"

namespace ShaderPool {

	std::vector<Shader*> _shaders;
	std::vector<std::string> _identifiers;

	void loadAndCompile(std::vector<std::string> paths)
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

			bool compiled = false;
			std::string name = shader_names[i];
			Shader* shader = new Shader(vertex_src, fragment_src, compiled, name);
			if (compiled)
			{
				_shaders.push_back(shader);
				_identifiers.push_back(name);
				Console::out::processInfo("Compiled " + name);
			}
		}

		Console::out::processDone("ShaderPool", "Finished building the shaders");
	}

	Shader* get(std::string name)
	{
		auto it = std::find(_identifiers.begin(), _identifiers.end(), name);
		if (it != _identifiers.end())
		{
			int32_t index = std::distance(_identifiers.begin(), it);
			return _shaders[index];
		}
		else
		{
			Console::out::error("ShaderPool", "Couldn't find shader " + name);
			return nullptr;
		}
	}

}