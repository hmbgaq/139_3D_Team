#include "CustomPxAssertHandler.h"

void CustomPxAssertHandler::operator()(const char* exp, const char* file, int line, bool& ignore)
{
	PX_UNUSED(ignore);
	printf("NV_CLOTH_ASSERT(%s) from file:%s:%d Failed\n", exp, file, line);
	assert(("Assertion failed, see log/console for more info.", 0));
}
