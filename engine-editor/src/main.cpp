#include "../src/runtime/runtime.h"

#include <filesystem>

namespace fs = std::filesystem;

int main()
{
	// TEMPORARY EMPTY EXAMPLE PROJECT PATH
	fs::path project = fs::current_path() / "examples" / "empty-project";

	// SYNCHRONOUSLY LOAD PROJECT
	Runtime::loadProject(project);

	// RUN EDITOR
	return Runtime::START_LOOP();
}