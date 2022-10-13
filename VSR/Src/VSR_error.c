#include "VSR_error.h"

static char* sErrorMessage;

const char* VSR_getErr()
{
	return sErrorMessage;
}


void VSR_SetErr(char* errMsg)
{
	sErrorMessage = sErrorMessage;
}

const char* VSR_VkErrorToString(VkResult result)
{
	const char* errStr = "NO_STRING_FOUND";
	switch ( result )
	{
		case VK_SUCCESS:
			errStr = "VK_SUCCESS";
			break;
		case VK_NOT_READY:
			errStr = "VK_NOT_READY";
			break;
		case VK_TIMEOUT:
			errStr = "VK_TIMEOUT";
			break;
		case VK_EVENT_SET:
			errStr = "VK_EVENT_SET";
			break;
		case VK_EVENT_RESET:
			errStr = "VK_EVENT_RESET";
			break;
		case VK_INCOMPLETE:
			errStr = "VK_INCOMPLETE";
			break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			errStr = "VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			errStr = "VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			errStr = "VK_ERROR_INITIALIZATION_FAILED";
			break;
		case VK_ERROR_DEVICE_LOST:
			errStr = "VK_ERROR_DEVICE_LOST";
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			errStr = "VK_ERROR_MEMORY_MAP_FAILED";
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			errStr = "VK_ERROR_LAYER_NOT_PRESENT";
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			errStr = "VK_ERROR_EXTENSION_NOT_PRESENT";
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			errStr = "VK_ERROR_FEATURE_NOT_PRESENT";
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			errStr = "VK_ERROR_INCOMPATIBLE_DRIVER";
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			errStr = "VK_ERROR_TOO_MANY_OBJECTS";
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			errStr = "VK_ERROR_FORMAT_NOT_SUPPORTED";
			break;
		case VK_ERROR_FRAGMENTED_POOL:
			errStr = "VK_ERROR_FRAGMENTED_POOL";
			break;
		case VK_ERROR_UNKNOWN:
			errStr = "VK_ERROR_UNKNOWN";
			break;
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			errStr = "VK_ERROR_OUT_OF_POOL_MEMORY";
			break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE:
			errStr = "VK_ERROR_INVALID_EXTERNAL_HANDLE";
			break;
		case VK_ERROR_FRAGMENTATION:
			errStr = "VK_ERROR_FRAGMENTATION";
			break;
		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			errStr = "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
			break;
		case VK_PIPELINE_COMPILE_REQUIRED:
			errStr = "VK_PIPELINE_COMPILE_REQUIRED";
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			errStr = "VK_ERROR_SURFACE_LOST_KHR";
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			errStr = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
			break;
		case VK_SUBOPTIMAL_KHR:
			errStr = "VK_SUBOPTIMAL_KHR";
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			errStr = "VK_ERROR_OUT_OF_DATE_KHR";
			break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			errStr = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
			break;
		case VK_ERROR_VALIDATION_FAILED_EXT:
			errStr = "VK_ERROR_VALIDATION_FAILED_EXT";
			break;
		case VK_ERROR_INVALID_SHADER_NV:
			errStr = "VK_ERROR_INVALID_SHADER_NV";
			break;
		case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
			errStr = "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
			break;
		case VK_ERROR_NOT_PERMITTED_KHR:
			errStr = "VK_ERROR_NOT_PERMITTED_KHR";
			break;
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			errStr = "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
			break;
		case VK_THREAD_IDLE_KHR:
			errStr = "VK_THREAD_IDLE_KHR";
			break;
		case VK_THREAD_DONE_KHR:
			errStr = "VK_THREAD_DONE_KHR";
			break;
		case VK_OPERATION_DEFERRED_KHR:
			errStr = "VK_OPERATION_DEFERRED_KHR";
			break;
		case VK_OPERATION_NOT_DEFERRED_KHR:
			errStr = "VK_OPERATION_NOT_DEFERRED_KHR";
			break;
		case VK_RESULT_MAX_ENUM:
			errStr = "VK_RESULT_MAX_ENUM";
			break;
		case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
			errStr = "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
			break;
	}
	
	return errStr;
}