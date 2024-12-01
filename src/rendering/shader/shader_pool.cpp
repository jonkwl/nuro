#include "shader_pool.h"

#include "../src/utils/log.h"
#include "../src/utils/iohandler.h"
#include "../src/rendering/shader/shader.h"

namespace ShaderPool {

	std::vector<Shader*> _shaders;
	std::vector<std::string> _identifiers;

	void loadAndCompile(std::vector<std::string> paths)
	{
		Log::printProcessStart("ShaderPool", "Building shaders...");
		Log::printProcessState("ShaderPool", "Fetching shaders...");

		std::vector<std::string> shader_paths;
		std::vector<std::string> shader_names;

		for (int i = 0; i < paths.size(); i++)
		{

			Log::printProcessInfo(paths[i] + ":");

			std::vector<std::string> shaders_in_folder = IOHandler::getFolders(paths[i]);
			for (int x = 0; x < shaders_in_folder.size(); x++)
			{

				shader_paths.push_back(paths[i] + "/" + shaders_in_folder[x]);
				shader_names.push_back(shaders_in_folder[x]);
				Log::printProcessInfo("- " + shader_names[shader_names.size() - 1]);
			}
		}

		Log::printProcessState("ShaderPool", "Compiling shaders...");
		for (int i = 0; i < shader_paths.size(); i++)
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
				Log::printProcessInfo("Compiled " + name);
			}
		}

		Log::printProcessDone("ShaderPool", "Finished building the shaders");
	}

	Shader* get(std::string name)
	{
		auto it = std::find(_identifiers.begin(), _identifiers.end(), name);
		if (it != _identifiers.end())
		{
			int index = std::distance(_identifiers.begin(), it);
			return _shaders[index];
		}
		else
		{
			Log::printError("ShaderPool", "Couldn't find shader " + name);
			return nullptr;
		}
	}

}