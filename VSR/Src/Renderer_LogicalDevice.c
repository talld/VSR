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

	///////////////
	/// aliases ///
	///////////////
	Renderer_LogicalDeviceCreateInfo * logicalDeviceCreateInfo =
		&subStructs->logicalDeviceCreateInfo;

	VkDeviceCreateInfo* deviceCreateInfo =
		&logicalDeviceCreateInfo->createInfo;

	Renderer_DeviceQueues* deviceQueues =
		&renderer->subStructs->deviceQueues;

	/////////////////////////////
	/// Create logical device ///
	/////////////////////////////
	VkDeviceQueueCreateInfo queueCreateInfos[kMaxUniqueQFamilies];

	for(size_t i = 0; i < kMaxUniqueQFamilies; i++)
	{

		// sort queues
		int isInUnique = 0;
		for (size_t j = 0; j < deviceQueues->uniqueQFamilyCount; j++)
		{
			if(deviceQueues->uniqueQFamilies[j] == deviceQueues->QFamilyIndexes[i])
			{
				isInUnique = 1;
			}
		}

		if(isInUnique == 0)
		{
			deviceQueues->uniqueQFamilies[deviceQueues->uniqueQFamilyCount] =
				deviceQueues->QFamilyIndexes[i];

			queueCreateInfos[deviceQueues->uniqueQFamilyCount].sType =
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

			queueCreateInfos[deviceQueues->uniqueQFamilyCount].pNext =
				NULL;

			queueCreateInfos[deviceQueues->uniqueQFamilyCount].flags =
				0L;

			queueCreateInfos[deviceQueues->uniqueQFamilyCount].pQueuePriorities =
				deviceQueues->queuePriorities;

			queueCreateInfos[deviceQueues->uniqueQFamilyCount].queueCount =
				1;

			queueCreateInfos[deviceQueues->uniqueQFamilyCount].queueFamilyIndex =
				deviceQueues->QFamilyIndexes[i];

			deviceQueues->uniqueQFamilyCount++;
		}
	}


	deviceCreateInfo->pQueueCreateInfos = queueCreateInfos;
	deviceCreateInfo->queueCreateInfoCount = deviceQueues->uniqueQFamilyCount;

	err = vkCreateDevice(renderer->subStructs->physicalDevice.device,
	                     deviceCreateInfo,
						 VSR_GetAllocator(),
						 &logicalDevice);

	if(err != VK_SUCCESS)
	{
        VSR_Error("Failed to create logical device: %s",
				VSR_VkErrorToString(err));
		goto FAIL;
	}

	renderer->subStructs->logicalDevice.device = logicalDevice;

	////////////////////
	/// Fetch queues ///
	////////////////////

	vkGetDeviceQueue(renderer->subStructs->logicalDevice.device,
	                 deviceQueues->QFamilies[kGraphicsQueueIndex],
	                 deviceQueues->QFamilyIndexes[kGraphicsQueueIndex],
					 &renderer->subStructs->deviceQueues.QList[kGraphicsQueueIndex]);

	vkGetDeviceQueue(renderer->subStructs->logicalDevice.device,
	                 deviceQueues->QFamilies[kTransferQueueIndex],
	                 deviceQueues->QFamilyIndexes[kTransferQueueIndex],
					 &renderer->subStructs->deviceQueues.QList[kTransferQueueIndex]);


	vkGetDeviceQueue(renderer->subStructs->logicalDevice.device,
	                 deviceQueues->QFamilies[kComputeQueueIndex],
	                 deviceQueues->QFamilyIndexes[kComputeQueueIndex],
					 &renderer->subStructs->deviceQueues.QList[kComputeQueueIndex]);

	if(renderer->subStructs->deviceQueues.QCanPresent[kGraphicsQueueIndex])
	{
		vkGetDeviceQueue(renderer->subStructs->logicalDevice.device,
		                 deviceQueues->QFamilies[kPresentQueueIndex],
		                 deviceQueues->QFamilyIndexes[kPresentQueueIndex],
		                 &renderer->subStructs->deviceQueues.QList[kPresentQueueIndex]);
	}
	else if(renderer->subStructs->deviceQueues.QCanPresent[kComputeQueueIndex])
	{
		vkGetDeviceQueue(renderer->subStructs->logicalDevice.device,
		                 deviceQueues->QFamilies[kGraphicsQueueIndex],
		                 deviceQueues->QFamilyIndexes[kComputeQueueIndex],
		                 &renderer->subStructs->deviceQueues.QList[kPresentQueueIndex]);
	}

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