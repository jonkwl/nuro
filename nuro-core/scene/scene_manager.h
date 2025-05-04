#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <utils/fsutil.h>

class Scene;

using SceneName = std::string;
using SceneRef = std::shared_ptr<Scene>;

class SceneManager {
public:
	SceneManager();
	
	// Creates an empty scene with a given name
	SceneName createScene(std::string name);

	// Creates a scene loaded from a scene file 
	SceneName createScene(FS::Path sceneFile);

	// Loads a scene by its name
	void loadScene(SceneName name);

private:
	std::unordered_map<SceneName, SceneRef> scenes;

};