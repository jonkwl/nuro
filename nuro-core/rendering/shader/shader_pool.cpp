
#include "shader_pool.h"

#include <thread>
#include <chrono>
#include <unordered_map>

#include <utils/console.h>
#include <rendering/shader/shader.h>
#include <context/application_context.h>

namespace ShaderPool {

	ResourceRef<Shader> gEmpty = std::make_shared<Shader>();
	std::unordered_map<std::string, ResourceRef<Shader>> gShaders;

	void _loadAll(const path& directory, bool async)
	{
		ResourceManager& resource = ApplicationContext::resourceManager();

		std::vector<path> shaderPaths;
		std::vector<std::string> shaderNames;

		std::vector<path> shadersInFolder = FSUtil::getFolders(directory);
		for (int32_t i = 0; i < shadersInFolder.size(); i++)
		{
			const path& shaderPath = shadersInFolder[i];
			shaderPaths.push_back(shaderPath);
			shaderNames.push_back(shaderPath.filename().string());
		}

		for (int32_t i = 0; i < shaderPaths.size(); i++)
		{
			// Get shaders identifier
			std::string identifier = shaderNames[i];

			// Skip shader creation if it already exists
			if (gShaders.find(identifier) != gShaders.end()) continue;

			// Create new shader and set its source
			auto& [shaderId, shader] = resource.create<Shader>(identifier + "_shader");
			shader->setSource(shaderPaths[i]);

			// Load shader
			if (async) {
				resource.exec(shader->create());
			}
			else {
				resource.execAsDependency(shader->create());
			}
			gShaders[identifier] = shader;
		}
	}

	void loadAllSync(const path& directory)
	{
		Console::out::info("Shader Pool", "Loading shaders from '" + directory.string() + "'");
		_loadAll(directory, false);
	}

	void loadAllAsync(const path& directory)
	{
		Console::out::info("Shader Pool", "Queued loading shader in '" + directory.string() + "'");
		_loadAll(directory, true);
	}

	ResourceRef<Shader> empty()
	{
		return gEmpty;
	}

	ResourceRef<Shader> get(const std::string& identifier)
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