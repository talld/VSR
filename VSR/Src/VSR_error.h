#ifndef VSR_ERROR_H
#define VSR_ERROR_H

#include <vulkan/vulkan.h>
#include <stdio.h>

void VSR_Error(const char* fmt, ...);

void VSR_SetErr(char* errMsg);

const char* VSR_VkErrorToString(VkResult result);

#ifdef VSR_DEBUG

#define VSR_LOG(...) \
	fprintf(stdout, "%s::%u ",__FILE__, __LINE__); \
	fprintf(stdout, __VA_ARGS__); \
	fprintf(stdout,"\n"); \
	fflush(stdout)

#else // VSR_DEBUG

#define VSR_LOG(...)

#endif // VSR_DEBUG

#endif // VSR_ERROR_H
