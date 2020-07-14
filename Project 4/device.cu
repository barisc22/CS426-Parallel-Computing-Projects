#include <stdio.h>
int main()
{
    // Get device properties
    cudaDeviceProp devProp;
    cudaGetDeviceProperties(&devProp, 0);
    printf("Name:                          %s\n",  devProp.name);
    printf("Total global memory:           %zu\n",  devProp.totalGlobalMem);
    printf("Total shared memory per block: %zu\n",  devProp.sharedMemPerBlock);
    printf("Total registers per block:     %d\n",  devProp.regsPerBlock);
    printf("Warp size:                     %d\n",  devProp.warpSize);
    printf("Maximum memory pitch:          %zu\n",  devProp.memPitch);
    printf("Maximum threads per block:     %d\n",  devProp.maxThreadsPerBlock);
    for (int i = 0; i < 3; ++i)
    printf("Maximum dimension %d of block:  %d\n", i, devProp.maxThreadsDim[i]);
    for (int i = 0; i < 3; ++i)
    printf("Maximum dimension %d of grid:   %d\n", i, devProp.maxGridSize[i]);
    printf("Clock rate:                    %d\n",  devProp.clockRate);
    printf("Total constant memory:         %zu\n",  devProp.totalConstMem);
    printf("Major revision number:         %d\n",  devProp.major);
    printf("Minor revision number:         %d\n",  devProp.minor);
    printf("Texture alignment:             %zu\n",  devProp.textureAlignment);
    printf("Concurrent copy and execution: %s\n",  (devProp.deviceOverlap ? "Yes" : "No"));
    printf("Number of multiprocessors:     %d\n",  devProp.multiProcessorCount);
    printf("Kernel execution timeout:      %s\n",  (devProp.kernelExecTimeoutEnabled ? "Yes" : "No"));    return 0;
    printf("Integrated:      %u\n",  (devProp.integrated));
    printf("Can Host Memory:      %u\n",  (devProp.canMapHostMemory));
    printf("Compute Mode:      %u\n",  (devProp.computeMode));
    printf("Concurrent Kernels:      %u\n",  (devProp.concurrentKernels));
    printf("ECCEnabled:      %u\n",  (devProp.ECCEnabled));
    printf("pciBusID:      %u\n",  (devProp.pciBusID));
    printf("pciDeviceID:      %u\n",  (devProp.pciDeviceID));
    printf("tccDriver:      %u\n",  (devProp.tccDriver));
}