
#include "shader_pool.h"

#include <unordered_map>

#include "../core/utils/console.h"
#include "../core/utils/iohandler.h"
#include "../core/rendering/shader/shader.h"
#include "../core/context/application_context.h"

#include <thread>
#include <chrono>

namespace ShaderPool {

	Shader* gEmpty = new Shader();
	std::unordered_map<std::string, Shader*> gShaders;

	void _loadAll(const std::string& directory, bool async)
	{
		ResourceLoader& loader = ApplicationContext::getResourceLoader();

		std::vector<std::string> shader_paths;
		std::vector<std::string> shader_names;

		std::vector<std::string> shaders_in_folder = IOHandler::getFolders(directory);
		for (int32_t x = 0; x < shaders_in_folder.size(); x++)
		{
			shader_paths.push_back(directory + "/" + shaders_in_folder[x]);
			shader_names.push_back(shaders_in_folder[x]);
		}

		for (int32_t i = 0; i < shader_paths.size(); i++)
		{
			// Get shaders identifier
			std::string identifier = shader_names[i];

			// Skip shader creation if it already exists
			if (gShaders.find(identifier) != gShaders.end()) continue;

			// Create new shader and set its source
			Shader* shader = new Shader();
			shader->setSource(shader_paths[i]);

			// Create shader
			if (async) {
				loader.createAsync(shader);
			}
			else {
				loader.createSync(shader);
			}
			gShaders[identifier] = shader;
		}
	}

	void loadAllSync(const std::string& directory)
	{
		Console::out::processStart("Shader Pool", "Loading shaders from '" + directory + "'");
		_loadAll(directory, false);
	}

	void loadAllAsync(const std::string& directory)
	{
		Console::out::processStart("Shader Pool", "Queued loading shader in '" + directory + "'");
		_loadAll(directory, true);
	}

	Shader* empty()
	{
		return gEmpty;
	}

	Shader* get(const std::string& identifier)
	{
		if (gShaders.find(identifier) != gShaders.end()) {
			// Shader available, return shader
			return gShaders[identifier];
		}
		else {
			// Shader not found, return empty shader
			Console::out::warning("Shader Pool", "Shader '" + identifier + "' was requested but does not exist!");
			return gEmpty;
		}
	}

}