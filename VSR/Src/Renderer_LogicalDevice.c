#include "Renderer_LogicalDevice.h"

#include "VSR_Renderer.h"
#include "VSR_error.h"

//==============================================================================
// VSR_RendererGenerateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_LogicalDevicePopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	Renderer_CreateInfoSubStructs* subStructs)
{
	///////////////
	/// aliases ///
	///////////////

	Renderer_PhysicalDeviceCreateInfo* physicalDeviceCreateInfo =
		&subStructs->physicalDeviceCreateInfo;

	Renderer_LogicalDeviceCreateInfo * logicalDeviceCreateInfo =
		&subStructs->logicalDeviceCreateInfo;

	Renderer_DeviceQueuesCreateInfo * deviceQueuesCreateInfo =
		&subStructs->deviceQueuesCreateInfo;

	VkDeviceCreateInfo* deviceCreateInfo =
		&logicalDeviceCreateInfo->createInfo;

	//////////////////////////////////////////////////////////
	/// Fill the features list with any features requested ///
	//////////////////////////////////////////////////////////

	if(createInfo->geometryShaderRequested)
	{
		subStructs->physicalDeviceCreateInfo.physicalDeviceFeatures2
		.features.geometryShader = VK_TRUE;
	}

	if(createInfo->tessellationShaderRequested)
	{
		physicalDeviceCreateInfo->physicalDeviceFeatures2.features.tessellationShader = VK_TRUE;
	}


	///////////////////////////////////////////
	/// Load queues into device create info ///
	///////////////////////////////////////////

	deviceCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo->flags = 0;
	deviceCreateInfo->pNext = NULL;

	deviceCreateInfo->pEnabledFeatures =
		&physicalDeviceCreateInfo->physicalDeviceFeatures2.features;

	static const char* deviceExtensions[1] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	deviceCreateInfo->enabledExtensionCount = 1;
	deviceCreateInfo->ppEnabledExtensionNames = deviceExtensions;
	deviceCreateInfo->enabledLayerCount = 0;
	deviceCreateInfo->ppEnabledLayerNames = NULL;

	// load the queue list
	deviceCreateInfo->queueCreateInfoCount = 3; // number MAY change in creation

	deviceCreateInfo->pQueueCreateInfos =
		deviceQueuesCreateInfo->queueCreateInfoList;



	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}





//==============================================================================
// VSR_SelectPhysicalDevice
//------------------------------------------------------------------------------
SDL_bool
VSR_LogicalDeviceCreate(
	VSR_Renderer* renderer,
	Renderer_CreateInfoSubStructs* subStructs)
{
	VkResult err;
	VkDevice logicalDevice;

	/////////////////////////////
	/// Create logical device ///
	/////////////////////////////
	err = vkCreateDevice(renderer->subStructs->physicalDevice.device,
						 &subStructs->logicalDeviceCreateInfo.createInfo,
						 VSR_GetAllocator(),
						 &logicalDevice);

	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create logical device: %s",
				VSR_VkErrorToString(err));

		VSR_SetErr(errMsg);
		goto FAIL;
	}

	renderer->subStructs->logicalDevice.device = logicalDevice;

	////////////////////
	/// Fetch queues ///
	////////////////////
	// FIXME: this is broken, handle queues better!

	vkGetDeviceQueue(renderer->subStructs->logicalDevice.device,
					 renderer->subStructs->deviceQueues.graphicsQueueFamilyIndex,
					 renderer->subStructs->deviceQueues.graphicsQueueIndex,
					 &renderer->subStructs->deviceQueues.graphicsQueue);

	// FIXME: this won't always be the case!
	vkGetDeviceQueue(renderer->subStructs->logicalDevice.device,
					 renderer->subStructs->deviceQueues.graphicsQueueFamilyIndex,
					 renderer->subStructs->deviceQueues.graphicsQueueIndex,
					 &renderer->subStructs->deviceQueues.presentQueue);

	vkGetDeviceQueue(renderer->subStructs->logicalDevice.device,
					 renderer->subStructs->deviceQueues.transferQueueFamilyIndex,
					 renderer->subStructs->deviceQueues.transferQueueIndex,
					 &renderer->subStructs->deviceQueues.transferQueue);


	vkGetDeviceQueue(renderer->subStructs->logicalDevice.device,
					 renderer->subStructs->deviceQueues.computeQueueFamilyIndex,
					 renderer->subStructs->deviceQueues.computeQueueIndex,
					 &renderer->subStructs->deviceQueues.computeQueue);

	///

	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}





//==============================================================================
// VSR_RendererGenerateCreateInfo
//------------------------------------------------------------------------------
void
VSR_LogicalDeviceDestroy(
	VSR_Renderer* renderer)
{
	vkDestroyDevice(renderer->subStructs->logicalDevice.device,
	                VSR_GetAllocator());
}





//==============================================================================
// VSR_LogicalDeviceFreeBuffer
//------------------------------------------------------------------------------
void
VSR_LogicalDeviceFreeMemory(
	VSR_Renderer* renderer,
	VkDeviceMemory memory)
{
	vkFreeMemory(renderer->subStructs->logicalDevice.device,
				 memory,
				 VSR_GetAllocator());
}