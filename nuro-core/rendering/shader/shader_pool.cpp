
#include "shader_pool.h"

#include <unordered_map>

#include <utils/console.h>
#include <utils/ioutils.h>
#include <rendering/shader/shader.h>
#include <context/application_context.h>

#include <thread>
#include <chrono>

namespace ShaderPool {

	Shader* gEmpty = new Shader();
	std::unordered_map<std::string, Shader*> gShaders;

	void _loadAll(const std::string& directory, bool async)
	{
		ResourceManager& resource = ApplicationContext::resourceManager();

		std::vector<std::string> shader_paths;
		std::vector<std::string> shader_names;

		std::vector<std::string> shaders_in_folder = IOUtils::getFolders(directory);
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
			auto [_, shader] = resource.create<Shader>();
			shader->setSource(shader_paths[i]);

			// Load shader
			if (async) {
				resource.loadAsync(shader);
			}
			else {
				resource.loadSync(shader);
			}
			gShaders[identifier] = shader;
		}
	}

	void loadAllSync(const std::string& directory)
	{
		Console::out::info("Shader Pool", "Loading shaders from '" + directory + "'");
		_loadAll(directory, false);
	}

	void loadAllAsync(const std::string& directory)
	{
		Console::out::info("Shader Pool", "Queued loading shader in '" + directory + "'");
		_loadAll(directory, true);
	}

	Shader* empty()
	{
		return gEmpty;
	}

	Shader* get(const std::string& identifier)
	{
		if (auto it = gShaders.find(identifier); it != gShaders.end()) {
			// Shader available, return shader
			return it->second;
		}
		else {
			// Shader not found, return empty shader
			Console::out::warning("Shader Pool", "Shader '" + identifier + "' was requested but does not exist!");
			return gEmpty;
		}
	}

}