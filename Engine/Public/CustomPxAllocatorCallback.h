#pragma once
#include "Base.h"

BEGIN(Engine)

class CustomPxAllocatorCallback : public physx::PxAllocatorCallback
{
	// PxAllocatorCallback을(를) 통해 상속됨
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line) override;
	virtual void deallocate(void* ptr) override;
};

END
