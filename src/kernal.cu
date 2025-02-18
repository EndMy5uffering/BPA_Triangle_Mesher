// kernel.cu
#include <cuda_runtime.h>
#include <iostream>

__global__ void myKernel() {
    printf("Hello from the GPU thread %d\n", threadIdx.x);
}

extern "C" void launchKernel() {
    myKernel<<<1, 10>>>();
    cudaDeviceSynchronize(); // Wait for the GPU to finish
}
