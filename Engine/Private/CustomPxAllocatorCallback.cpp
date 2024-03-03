#include "CustomPxAllocatorCallback.h"

void* CustomPxAllocatorCallback::allocate(size_t size, const char* typeName, const char* filename, int line)
{
	void* ptr = platformAlignedAlloc(size);
	PX_ASSERT((reinterpret_cast<size_t>(ptr) & 15) == 0);
	return ptr;
}

void CustomPxAllocatorCallback::deallocate(void* ptr)
{
	platformAlignedFree(ptr);
}
