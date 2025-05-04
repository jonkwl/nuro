#include "scene_manager.h"

#include "scene.h"

SceneManager::SceneManager() : scenes() 
{
}

SceneName SceneManager::createScene(std::string name)
{
	return SceneName();
}

SceneName SceneManager::createScene(FS::Path sceneFile)
{
	return SceneName();
}

void SceneManager::loadScene(SceneName name)
{
}
