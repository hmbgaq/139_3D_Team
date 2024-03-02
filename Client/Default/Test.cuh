#pragma once
#include <cuda_runtime.h>
#ifdef __cplusplus
extern "C" {//<-- extern 
#endif
	class CGPUACC	
	{	
		public:
			CGPUACC(void);		
			virtual ~CGPUACC(void);	
			int sum_cuda(int a, int b, int* c);
	};
#ifdef __cplusplus 
}
#endif

