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
// VSR_LogicalDeviceCreateBuffer
//------------------------------------------------------------------------------
VkBuffer
VSR_LogicalDeviceCreateBuffer(
	VSR_Renderer* renderer,
	size_t size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties)
{
	VkBuffer buffer;

	VkBufferCreateInfo bufferInfo = (VkBufferCreateInfo){0};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = NULL;
	bufferInfo.flags = 0L;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult err =
	vkCreateBuffer(renderer->subStructs->logicalDevice.device,
				   &bufferInfo,
				   VSR_GetAllocator(),
				   &buffer);

	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create buffer: %s",
				VSR_VkErrorToString(err));

		VSR_SetErr(errMsg);
	}

	VkMemoryRequirements memReq = (VkMemoryRequirements){0};
	vkGetBufferMemoryRequirements(renderer->subStructs->logicalDevice.device,
								  buffer,
								  &memReq);

	VkPhysicalDeviceMemoryProperties memoryProperties;

	vkGetPhysicalDeviceMemoryProperties(
		renderer->subStructs->physicalDevice.device,
		&memoryProperties);

	size_t memIndex = -1;
	for(size_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if((memReq.memoryTypeBits) & (1 << i )
		&& (properties & memoryProperties.memoryTypes[i].propertyFlags) == properties)
		{
			memIndex = i;
			break;
		}
	}

	VkMemoryAllocateInfo allocateInfo = (VkMemoryAllocateInfo){0};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = NULL;
	allocateInfo.allocationSize = memReq.size;
	allocateInfo.memoryTypeIndex = memIndex;

	VkDeviceMemory deviceMemory;
	vkAllocateMemory(renderer->subStructs->logicalDevice.device,
					 &allocateInfo,
					 VSR_GetAllocator(),
					 &deviceMemory);

	vkBindBufferMemory(renderer->subStructs->logicalDevice.device,
					   buffer,
					   deviceMemory,
					   0);

	return buffer;
}





//==============================================================================
// VSR_LogicalDeviceFreeBuffer
//------------------------------------------------------------------------------
void
VSR_LogicalDeviceFreeBuffer(
	VSR_Renderer* renderer,
	VkBuffer buffer)
{
	vkDestroyBuffer(renderer->subStructs->logicalDevice.device,
					buffer,
					VSR_GetAllocator());
}