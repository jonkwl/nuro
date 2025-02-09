#include "../src/runtime/runtime.h"

int main()
{
	Runtime::loadProject("C:/Users/jonko/Dokumente/development/nuro/editor-runtime/src/example");
	return Runtime::START_LOOP();
}