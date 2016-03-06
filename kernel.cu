#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <iostream>
#include <string>
#include <sstream>

//#define NDEBUG

#include <CustomUtils\BenchmarkTimerForWindows.h>
#include <CustomUtils\Cusassertion.h>
#include <CustomUtilsCUDA\displayDeviceInfo.h>

const static float eps = 1e-6;
const static size_t blocSize = 8;
const static size_t size = 1024;

__global__ void matMultiply1D(float* matA, float* matB, float* Dest, int dimensions)
{
	int i = threadIdx.x + blockIdx.x*blockDim.x;
	if (i < dimensions)
	{
		float vectA[2048];
		for (unsigned k = 0; k != dimensions; ++k)
		{
			vectA[k] = matB[i*dimensions + k];
		}
		for (unsigned j = 0; j != dimensions; ++j)
		{
			float res = 0.0f;
			for (unsigned k = 0; k != dimensions; ++k)
			{
				res += vectA[k] * matB[k*dimensions + j];
			}
			Dest[i*dimensions + j] = res;
		}
	}
}

__global__ void matMultiply2D(float* matA, float* matB, float* Dest, int dimensions)
{
	int ix = threadIdx.x + blockIdx.x*blockDim.x;
	int iy = threadIdx.y + blockIdx.y*blockDim.y;

	if (ix < dimensions&&iy < dimensions)
	{
		float res = 0.0f;
		for (unsigned k = 0; k != dimensions; ++k)
		{
			res += matA[ix*dimensions + k] * matB[k*dimensions + iy];
		}
		Dest[ix*dimensions + iy] = res;
	}
}

int main(void)
{
	cudaError_t err;
	float *A = new float[size*size];
	float *B = new float[size*size];
	float *C = new float[size*size];
	for (unsigned i = 0; i != size; ++i)
	{
		for (unsigned j = 0; j != size; ++j)
		{
			A[i*size + j] = B[i*size + j] = (rand() % 1000) / 1000.0f;
		}
	}

	err = cudaSetDevice(0);
	displayDeviceInfo(0);
	float *matA = nullptr;
	float *matB = nullptr;
	float *matC = nullptr;
	err = cudaMalloc((void**)&matA, sizeof(float)*size*size);
	err = cudaMalloc((void**)&matB, sizeof(float)*size*size);
	err = cudaMalloc((void**)&matC, sizeof(float)*size*size);

	err = cudaMemcpy(matA, A, sizeof(float)*size*size, cudaMemcpyHostToDevice);
	err = cudaMemcpy(matB, B, sizeof(float)*size*size, cudaMemcpyHostToDevice);

	BenchmarkTimer Benchmarker;
	Benchmarker.startTimer();

	size_t blockCount = size / blocSize;
	if (blockCount - blockCount*blocSize > 0)
	{
		blockCount += 1;
	}
	//matMultiply1D <<<blockCount, blocSize >>>(matA, matB, matC, size);
	dim3 blockSetup(size / blocSize, size / blocSize);
	dim3 blockSizeSetup(blocSize, blocSize);
	matMultiply2D << <blockSetup, blockSizeSetup >> >(matA, matB, matC, size);
	err = cudaDeviceSynchronize();

	Benchmarker.endTimer();
	std::cout << "Kernel exec time elapsed: " << Benchmarker.getDeltaTimeInms() << " ms(s)\n";

	err = cudaMemcpy(C, matC, sizeof(float)*size*size, cudaMemcpyDeviceToHost);
	err = cudaGetLastError();
	CusAssertion(err == cudaSuccess, "CUDA runtime error caught: " << cudaGetErrorString(err));
	std::cout << "Result retrieved" << std::endl;

	Benchmarker.startTimer();
	for (unsigned i = 0; i != size; ++i)
	{
		for (unsigned j = 0; j != size; ++j)
		{
			float res = 0.0f;
			for (unsigned k = 0; k != size; ++k)
			{
				res += A[i*size + k] * B[k*size + j];
			}
			CusAssertion(abs(res - C[i*size + j]) < eps, "res is not equal to C " << i << "," << j);
		}
	}
	Benchmarker.endTimer();
	std::cout << "Host exec time: " << Benchmarker.getDeltaTimeInms() << " ms(s)\n";

	delete[] A;
	delete[] B;
	delete[] C;

	err = cudaFree((void**)&matA);
	err = cudaFree((void**)&matB);
	err = cudaFree((void**)&matC);

	std::cout << "Result correct" << std::endl;

	cudaDeviceReset();
}