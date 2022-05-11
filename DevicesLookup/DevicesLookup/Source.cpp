#include <conio.h>
#include <stdio.h>

#include "CL/opencl.h"
#include "CL/cl.hpp"


const char* GetError(cl_int error);
const char* GetDevType(cl_device_type type);

void main()
{

	int i = 0;
	cl_int err;
	cl_platform_id		*platforms;
	cl_device_id		*devices;

	cl_uint				nPlatforms = 0;
	cl_uint				nDevices = 0;

	cl_context			clContext;
	cl_command_queue	clQueue;

	cl_int dev_id = 0;

	printf(" ::: OpenCL devices look-up, v1.0 :::\n");
	printf(" :                                  :\n");
	printf(" :::::       DEVICES LIST       :::::\n");
	printf(" :                                  :\n");
	printf(" ::::::::::::::::::::::::::::::::::::\n\n\n");

	err = clGetPlatformIDs(NULL, NULL, &nPlatforms);
	if(err)
		printf("clGetPlatformIDs() Error: %s\n\n", GetError(err));
	

	platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id)*(nPlatforms));
	err = clGetPlatformIDs(nPlatforms, platforms, &nPlatforms);	
	if(err) printf("\nclGetPlatformIDs (platforms) Error: %s\n", GetError(err));

	char Name[128] = {0};
	char Buff[128] = {0};

	for(i = 0; i<nPlatforms; i++)
	{
		printf(" %d: ", i);
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 128*sizeof( char ), Name, NULL);
		if(err)
			printf("clGetPlatformInfo (CL_PLATFORM_NAME) Error: %s\n", GetError(err));

		err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, NULL, NULL, &nDevices); 
		if(err)
			printf("clGetDeviceIDs (numbers) Error: %s\n", GetError(err));
		else
			printf("%s (%d devices)\n", Name, nDevices);
		
		devices = (cl_device_id *)malloc(sizeof(cl_device_id)*(nDevices));
		err = clGetDeviceIDs( platforms[i], CL_DEVICE_TYPE_ALL, nDevices, devices, &nDevices);
		
		cl_uint ret_ui;
		cl_ulong ret_ul;
		cl_device_type type;
		size_t wsz[3];

		for(int j = 0; j<nDevices; j++)
		{
			//printf("Device ID is %d\n", j);
			err = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 128 * sizeof( char ), Name, NULL );
			if(err) printf("clGetDeviceInfo Error: %s\n", GetError(err));
			clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(cl_device_type), &type, NULL );
			printf("  [%d] :: %s (%s) - (%d, %d)\n", dev_id, Name, GetDevType(type), i, j);
			dev_id++;
		}
		printf("\n");
		free(devices);
	}
	printf("\n");
	free(platforms);

	printf("  For selecting devices, create a file [deleted].ini.\n");
	printf("  Put there device numbers found in [] devided by spaces, like this:\n");
	printf("        X1 X2 ... XN     where XX - device's number. \n\n");



	printf("Finished.\n");
	_getch();

}




const char* GetError(cl_int error)
{
	switch (error)
	{
	case CL_SUCCESS:
		return "CL_SUCCESS";
	case CL_DEVICE_NOT_FOUND: 
		return "CL_DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:	
		return "CL_DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:	
		return "CL_COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:				
		return "CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:				
		return "CL_OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:	
		return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:				
		return "CL_MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:			
		return "CL_IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:		
		return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:			
		return "CL_BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:					
		return "CL_MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:	
		return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
		return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case CL_KERNEL_ARG_INFO_NOT_AVAILABLE: 
		return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

	case CL_INVALID_VALUE: 
		return "CL_INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE: 
		return "CL_INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:
		return "CL_INVALID_PLATFORM";
	case CL_INVALID_DEVICE:
		return "CL_INVALID_DEVICE";
	case CL_INVALID_CONTEXT:
		return "CL_INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:
		return "CL_INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE: 
		return "CL_INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR: 
		return "CL_INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT: 
		return "CL_INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: 
		return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:
		return "CL_INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:
		return "CL_INVALID_SAMPLER";
	case CL_INVALID_BINARY:
		return "CL_INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:
		return "CL_INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM: 
		return "CL_INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE: 
		return "CL_INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME: 
		return "CL_INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION: 
		return "CL_INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL: 
		return "CL_INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:
		return "CL_INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:
		return "CL_INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE: 
		return "CL_INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS: 
		return "CL_INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION: 
		return "CL_INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE: 
		return "CL_INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE: 
		return "CL_INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET: 
		return "CL_INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST: 
		return "CL_INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:
		return "CL_INVALID_EVENT";
	case CL_INVALID_OPERATION:
		return "CL_INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT: 
		return "CL_INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE: 
		return "CL_INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL: 
		return "CL_INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE: 
		return "CL_INVALID_GLOBAL_WORK_SIZE";
	default:
		return "UNKNOWN_ERROR";
	}
}


const char* GetDevType(cl_device_type type)
{
	switch (type)
	{
	case CL_DEVICE_TYPE_DEFAULT:
		return "DEFAULT";
	case CL_DEVICE_TYPE_CPU: 
		return "CPU";
	case CL_DEVICE_TYPE_GPU:	
		return "GPU";
	case CL_DEVICE_TYPE_ACCELERATOR:	
		return "ACCELERATOR";
	case CL_DEVICE_TYPE_CUSTOM:
		return "CUSTOM";
	default:
		return "UNKNOWN";
	}
}
