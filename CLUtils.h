#include <CL/opencl.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <iomanip>
#include <time.h>
//Maybe we should use CustomizedAssertion instead
#define CLDebug
#define PerformanceCheck

const char* getErrorString(cl_int error)
{
	static const char* strings[] =
	{
		// Error Codes
		"CL_SUCCESS"                                  //   0
		, "CL_DEVICE_NOT_FOUND"                         //  -1
		, "CL_DEVICE_NOT_AVAILABLE"                     //  -2
		, "CL_COMPILER_NOT_AVAILABLE"                   //  -3
		, "CL_MEM_OBJECT_ALLOCATION_FAILURE"            //  -4
		, "CL_OUT_OF_RESOURCES"                         //  -5
		, "CL_OUT_OF_HOST_MEMORY"                       //  -6
		, "CL_PROFILING_INFO_NOT_AVAILABLE"             //  -7
		, "CL_MEM_COPY_OVERLAP"                         //  -8
		, "CL_IMAGE_FORMAT_MISMATCH"                    //  -9
		, "CL_IMAGE_FORMAT_NOT_SUPPORTED"               //  -10
		, "CL_BUILD_PROGRAM_FAILURE"                    //  -11
		, "CL_MAP_FAILURE"                              //  -12

		, ""    //  -13
		, ""    //  -14
		, ""    //  -15
		, ""    //  -16
		, ""    //  -17
		, ""    //  -18
		, ""    //  -19

		, ""    //  -20
		, ""    //  -21
		, ""    //  -22
		, ""    //  -23
		, ""    //  -24
		, ""    //  -25
		, ""    //  -26
		, ""    //  -27
		, ""    //  -28
		, ""    //  -29

		, "CL_INVALID_VALUE"                            //  -30
		, "CL_INVALID_DEVICE_TYPE"                      //  -31
		, "CL_INVALID_PLATFORM"                         //  -32
		, "CL_INVALID_DEVICE"                           //  -33
		, "CL_INVALID_CONTEXT"                          //  -34
		, "CL_INVALID_QUEUE_PROPERTIES"                 //  -35
		, "CL_INVALID_COMMAND_QUEUE"                    //  -36
		, "CL_INVALID_HOST_PTR"                         //  -37
		, "CL_INVALID_MEM_OBJECT"                       //  -38
		, "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"          //  -39
		, "CL_INVALID_IMAGE_SIZE"                       //  -40
		, "CL_INVALID_SAMPLER"                          //  -41
		, "CL_INVALID_BINARY"                           //  -42
		, "CL_INVALID_BUILD_OPTIONS"                    //  -43
		, "CL_INVALID_PROGRAM"                          //  -44
		, "CL_INVALID_PROGRAM_EXECUTABLE"               //  -45
		, "CL_INVALID_KERNEL_NAME"                      //  -46
		, "CL_INVALID_KERNEL_DEFINITION"                //  -47
		, "CL_INVALID_KERNEL"                           //  -48
		, "CL_INVALID_ARG_INDEX"                        //  -49
		, "CL_INVALID_ARG_VALUE"                        //  -50
		, "CL_INVALID_ARG_SIZE"                         //  -51
		, "CL_INVALID_KERNEL_ARGS"                      //  -52
		, "CL_INVALID_WORK_DIMENSION"                   //  -53
		, "CL_INVALID_WORK_GROUP_SIZE"                  //  -54
		, "CL_INVALID_WORK_ITEM_SIZE"                   //  -55
		, "CL_INVALID_GLOBAL_OFFSET"                    //  -56
		, "CL_INVALID_EVENT_WAIT_LIST"                  //  -57
		, "CL_INVALID_EVENT"                            //  -58
		, "CL_INVALID_OPERATION"                        //  -59
		, "CL_INVALID_GL_OBJECT"                        //  -60
		, "CL_INVALID_BUFFER_SIZE"                      //  -61
		, "CL_INVALID_MIP_LEVEL"                        //  -62
		, "CL_INVALID_GLOBAL_WORK_SIZE"                 //  -63
		, "CL_UNKNOWN_ERROR_CODE"
	};

	if (error >= -63 && error <= 0)
		return strings[-error];
	else
		return strings[64];
}

class CLBufferObject
{
public:
	cl_mem memBuffer;
	void createBuffer(cl_context context, size_t size, cl_mem_flags access = CL_MEM_READ_WRITE, void* data = nullptr)
	{
		cl_int status = 0;
		memBuffer = clCreateBuffer(context, access, size, data, &status);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened creating the buffer object" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
		}
#endif
	}

	void feedBuffer(cl_command_queue cmdQueue, size_t size, void* data, cl_bool blockflag = CL_FALSE)
	{
		cl_int status = clEnqueueWriteBuffer(cmdQueue, this->memBuffer, blockflag, 0, size, data, 0, nullptr, nullptr);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened feeding the buffer object" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
		}
#endif
	}

	void fetchBuffer(cl_command_queue cmdQueue, size_t size, void* dest, cl_bool blockFlag = CL_TRUE)
	{
		cl_int status = clEnqueueReadBuffer(cmdQueue, this->memBuffer, blockFlag, 0, size, dest, 0, nullptr, nullptr);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened retrieving the buffer memory" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
		}
#endif
	}

	void releaseBuffer()
	{
		clReleaseMemObject(this->memBuffer);
		std::cout << "Buffer " << this->memBuffer << " released!" << std::endl;
	}
};

class CLCmdQueueObject
{
public:
	cl_device_id toDevice;
	cl_command_queue theQueue;

	void Attach(cl_context Context, cl_device_id deviceID)
	{
		cl_int status = 0;
		toDevice = deviceID;
#ifdef PerformanceCheck
		theQueue = clCreateCommandQueue(Context, deviceID, CL_QUEUE_PROFILING_ENABLE, &status);
#endif
#ifndef PerformanceCheck
		theQueue = clCreateCommandQueue(Context, deviceID, 0, &status);
#endif
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened creating the command queue" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
		}
#endif
	}
};

class CLUtilsObject
{
public:
	cl_platform_id *platforms;
	cl_uint numPlatforms;
	
	cl_device_id *devices;
	cl_uint numDevices;

	cl_context curContext;

	cl_program curProgram;

	std::map<std::string, cl_kernel> kernels;
	std::map<cl_device_id, CLCmdQueueObject> cmdQueue;
	std::vector<cl_mem> buffers;

	void CLPlatformStartup()
	{
		cl_int status = 0;
		status = clGetPlatformIDs(0, nullptr, &this->numPlatforms);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened while retrieving platform numbers" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
			exit(-1);
		}
		if (this->numPlatforms < 1)
		{
			std::cerr << "No platform available was found" << std::endl;
			exit(-1);
		}
#endif
		platforms = new cl_platform_id[numPlatforms];
		status = clGetPlatformIDs(this->numPlatforms, platforms, nullptr);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened while retrieving platform infos" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
			exit(-1);
		}
#endif
		size_t stringSize = 0;
	}

	void CLDeviceStartup(unsigned platformIndex)
	{
		if (platformIndex >= this->numPlatforms)
		{
			std::cout << "Platform index out of range " << std::endl;
			return;
		}

		cl_int status = 0;
		size_t strSize = 0;

		status = clGetPlatformInfo(this->platforms[platformIndex], CL_PLATFORM_NAME, 0, nullptr, &strSize);
		char* Name = new char[strSize];
		status = clGetPlatformInfo(this->platforms[platformIndex], CL_PLATFORM_NAME, strSize, Name, nullptr);

		std::cout << "Establishing on platform: " << Name << std::endl;
		delete[] Name;

		status = clGetPlatformInfo(this->platforms[platformIndex], CL_PLATFORM_VENDOR, 0, nullptr, &strSize);
		Name = new char[strSize];
		status = clGetPlatformInfo(this->platforms[platformIndex], CL_PLATFORM_VENDOR, strSize, Name, nullptr);

		std::cout << "Platform is provided by: " << Name << std::endl;
		delete[] Name;

		status = clGetPlatformInfo(this->platforms[platformIndex], CL_PLATFORM_PROFILE, 0, nullptr, &strSize);
		Name = new char[strSize];
		status = clGetPlatformInfo(this->platforms[platformIndex], CL_PLATFORM_PROFILE, strSize, Name, nullptr);

		std::cout << "Platform support info: " << Name << std::endl;
		delete[] Name;

		status = clGetDeviceIDs(this->platforms[platformIndex], CL_DEVICE_TYPE_ALL, 0, nullptr, &this->numDevices);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened while retrieving device numbers on the platform" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
			exit(-1);
		}
		if (this->numDevices < 1)
		{
			std::cerr << "No devices available were found on the platform" << std::endl;
			exit(-1);
		}
#endif
		this->devices = new cl_device_id[numDevices];
		status = clGetDeviceIDs(this->platforms[platformIndex], CL_DEVICE_TYPE_ALL, this->numDevices, this->devices, nullptr);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened while retrieving device infos on the platform" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
			exit(-1);
		}
#endif
	}

	void CLContextStartup()
	{
		cl_int status = 0;
		this->curContext = clCreateContext(nullptr, this->numDevices, this->devices, nullptr, nullptr, &status);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened while creating context" << std::endl;
			std::cerr << getErrorString(status) << std::endl;
			exit(-1);
		}
#endif
	}

	void CLQuickStartup()
	{
		CLPlatformStartup();
		CLDeviceStartup(0);
		CLContextStartup();
	}

	void AddcmdQueue(cl_device_id toDevice)
	{
		CLCmdQueueObject Queue;
		Queue.Attach(this->curContext, toDevice);
		this->cmdQueue.emplace(std::pair<cl_device_id, CLCmdQueueObject>(toDevice, Queue));
	}

	void AddcmdQueue(unsigned deviceIndex = 0)
	{
		CLCmdQueueObject Queue;
		Queue.Attach(this->curContext, this->devices[deviceIndex]);
		//this->cmdQueue.push_back(Queue);
		this->cmdQueue.emplace(std::pair<cl_device_id, CLCmdQueueObject>(this->devices[deviceIndex], Queue));
	}

	void ProgFromSource(const char* srcCode)
	{
		cl_int status = 0;
		this->curProgram = clCreateProgramWithSource(this->curContext, 1, (const char**)&srcCode, nullptr, &status);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened creating the program from source" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
			exit(-1);
		}
#endif

		status = clBuildProgram(this->curProgram, this->numDevices, this->devices, nullptr, nullptr, nullptr);
#ifdef CLDebug
		if (status != 0)
		{
			char buildLog[16384];
			for (unsigned i = 0; i != this->numDevices; ++i)
			{
				clGetProgramBuildInfo(this->curProgram, this->devices[i], CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, nullptr);
				std::cout << "Device: " << this->devices[i] << " reported build error:\n" << buildLog << std::endl;
			}
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
			exit(-1);
		}
#endif
	}

	void ProgFromFile(const char* filePath)
	{
		cl_int status = 0;
		std::fstream fin(filePath, std::ios::in);

		if (!fin.is_open())
		{
			std::cout << "File: " << filePath << " Does not exist!" << std::endl;
			return;
		}

		fin.seekg(0, std::ios::end);
		size_t fileSize = fin.tellg();
		fin.seekg(0);

		char* srcCode = new char[fileSize + 1];
		fin.read(srcCode, fileSize);
		size_t offset = 0;
		while (srcCode[offset] >= 0)
		{
			++offset;
		}
		srcCode[offset] = 0;

		this->curProgram = clCreateProgramWithSource(this->curContext, 1, (const char**)&srcCode, nullptr, &status);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened creating the program from source" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
			exit(-1);
		}
#endif

		delete[] srcCode;
		fin.close();

		status = clBuildProgram(this->curProgram, this->numDevices, this->devices, nullptr, nullptr, nullptr);
#ifdef CLDebug
		if (status != 0)
		{
			char buildLog[16384];
			for (unsigned i = 0; i != this->numDevices; ++i)
			{
				clGetProgramBuildInfo(this->curProgram, this->devices[i], CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, nullptr);
				std::cout << "Device: " << this->devices[i] << " reported build error:\n" << buildLog << std::endl;
			}
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
			exit(-1);
		}
#endif
	}

	void AddKernel(const char* kernelName)
	{
		cl_int status = 0;
		cl_kernel tmpKer = clCreateKernel(this->curProgram, kernelName, &status);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened while adding a kernel" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
			exit(-1);
		}
#endif
		this->kernels.emplace(std::pair<std::string, cl_kernel>(std::string(kernelName), tmpKer));
	}

	void FeedKerArgs(const char* kerName, cl_uint argumentNumber, size_t size, const void* data)
	{
		cl_int status = clSetKernelArg(this->kernels[kerName], argumentNumber, size, data);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened while feeding kernel arguments" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
			exit(-1);
		}
#endif
	}

	void EnqueueNDKernel(cl_device_id device, const char* kernelName, unsigned Ndimensions, const size_t *globalWorkSize, const size_t *localWorkSize)
	{
		cl_event profile_event = 0;
		cl_int status = clEnqueueNDRangeKernel(this->cmdQueue.at(device).theQueue, this->kernels[kernelName], Ndimensions, nullptr, globalWorkSize, localWorkSize, 0, nullptr, &profile_event);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened while enqueueing NDRange kernel executions" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
			exit(-1);
		}
#endif
		std::cout << "Kernel: " << kernelName << " enqueued at " << Ndimensions << " dimension(s) at queue: "
			<< this->cmdQueue.at(device).theQueue << ", the queue's currently attached to device: " << device << std::endl;
#ifdef PerformanceCheck
		cl_ulong timeStart = 0;
		cl_ulong timeEnd = 0;
		clFinish(this->cmdQueue.at(device).theQueue);
		status = clGetEventProfilingInfo(profile_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &timeStart, nullptr);
		status = clGetEventProfilingInfo(profile_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &timeEnd, nullptr);
		std::cout << std::setprecision(14);
		std::cout << "Time elapsed: " << (double)(timeEnd - timeStart) / 1e6 << " in millisecond(s)" << std::endl;
#endif
	}

	void EnqueueNDKernel(cl_command_queue cmdQueue, const char* kernelName, unsigned Ndimensions, const size_t *globalWorkSize, const size_t *localWorkSize)
	{
		cl_event profile_event = 0;
		cl_int status = clEnqueueNDRangeKernel(cmdQueue, this->kernels[kernelName], Ndimensions, nullptr, globalWorkSize, localWorkSize, 0, nullptr, &profile_event);
#ifdef CLDebug
		if (status != 0)
		{
			std::cerr << "Something happened while enqueueing NDRange kernel executions" << std::endl;
			std::cerr << "Error description: " << getErrorString(status) << std::endl;
			exit(-1);
		}
#endif
		std::cout << "Kernel: " << kernelName << " enqueued at " << Ndimensions << " dimension(s) at queue: "
			<< cmdQueue << std::endl;
#ifdef PerformanceCheck
		cl_ulong timeStart = 0;
		cl_ulong timeEnd = 0;
		clFinish(cmdQueue);
		status = clGetEventProfilingInfo(profile_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &timeStart, nullptr);
		status = clGetEventProfilingInfo(profile_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &timeEnd, nullptr);
		std::cout << std::setprecision(14);
		std::cout << "Time elapsed: " << (double)(timeEnd - timeStart) / 1e6 << " in millisecond(s)" << std::endl;
#endif
	}

	void CleanUp()
	{
		for (auto a : this->kernels)
		{
			clReleaseKernel(a.second);
			std::cout << "Kernel " << '"' << a.first << '"' << " dumped!" << std::endl;
		}
		this->kernels.clear();
		
		clReleaseProgram(this->curProgram);
		
		for (auto a : this->cmdQueue)
		{
			clReleaseCommandQueue(a.second.theQueue);
			std::cout << "Queue connected to device " << a.first << " dumped!" << std::endl;
		}
		this->cmdQueue.clear();

		clReleaseContext(this->curContext);
		std::cout << "Context dumped!" << std::endl;

		delete[] this->devices;
		delete[] this->platforms;
	}
};