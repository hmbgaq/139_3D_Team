#include"Test.cuh"
#include"cuda.h"
#include<iostream>
#include<cufft.h>
#include"cublas_v2.h"
#include<stdio.h>
#include<stdlib.h>

CGPUACC::CGPUACC()
{

}

CGPUACC::~CGPUACC()
{

}

__global__ void sum_kernel(int a, int b, int* c)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	c[tid] = a + b;
}

int CGPUACC::sum_cuda(int a, int b, int* c)
{
	int* f;

	cudaMalloc((void**)&f, sizeof(int) * 1);
	cudaMemcpy(f, c, sizeof(int) * 1, cudaMemcpyHostToDevice);

	sum_kernel <<<1, 1 >>> (a, b, f);

	cudaMemcpy(c, f, sizeof(int) * 1, cudaMemcpyDeviceToHost);

	cudaFree(f);

	return true;
}
