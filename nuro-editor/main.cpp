#include "../runtime/runtime.h"

#include <filesystem>

namespace fs = std::filesystem;

int main()
{
	// RUN EDITOR
	return Runtime::START_LOOP();
}