#include "../src/runtime/runtime.h"

int main()
{
	Runtime::loadProject(PROJECT_PATH);
	return Runtime::START_LOOP();
}