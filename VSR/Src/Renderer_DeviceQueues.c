#include "Renderer_DeviceQueues.h"

#include "VSR_Renderer.h"

SDL_bool VSR_DeviceQueuesPopulateCreateInfo(
	VSR_RendererCreateInfo* rendererCreateInfo,
	Renderer_CreateInfoSubStructs* subStructs)
{
	///////////////
	/// Aliases ///
	///////////////
	Renderer_DeviceQueuesCreateInfo* deviceQueuesCreateInfo =
		&subStructs->deviceQueuesCreateInfo;

	VkDeviceQueueCreateInfo* createInfoList =
		subStructs->deviceQueuesCreateInfo.queueCreateInfoList;

	/////////////////////
	/// Setup queues  ///
	/////////////////////

	// we're only going to use 3 queues to make things simple
	// 1 transfer queue
	// 2 graphics queue
	// 3 compute queue
	// managing queuePriority individually may be to complex, fix it for now
	static const float globalQueuePriority[3] = {1.0f,1.0f,1.0f};

	deviceQueuesCreateInfo->transferQueueCreateInfoIndex = 2;
	deviceQueuesCreateInfo->computeQueueCreateInfoIndex = 1;
	deviceQueuesCreateInfo->graphicsQueueCreateInfoIndex = 0;


	VkDeviceQueueCreateInfo* graphicsQueueCreateInfo =
		&createInfoList[deviceQueuesCreateInfo->graphicsQueueCreateInfoIndex];

	graphicsQueueCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphicsQueueCreateInfo->pNext = NULL;
	graphicsQueueCreateInfo->flags = 0;
	graphicsQueueCreateInfo->pQueuePriorities = globalQueuePriority;
	graphicsQueueCreateInfo->queueCount = 1;
	graphicsQueueCreateInfo->queueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;


	VkDeviceQueueCreateInfo* computeQueueCreateInfo =
		&createInfoList[deviceQueuesCreateInfo->computeQueueCreateInfoIndex];

	computeQueueCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	computeQueueCreateInfo->pNext = NULL;
	computeQueueCreateInfo->flags = 0;
	computeQueueCreateInfo->pQueuePriorities = globalQueuePriority;
	computeQueueCreateInfo->queueCount = 1;
	computeQueueCreateInfo->queueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;


	VkDeviceQueueCreateInfo* transferQueueCreateInfo =
		&createInfoList[deviceQueuesCreateInfo->transferQueueCreateInfoIndex];

	transferQueueCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	transferQueueCreateInfo->pNext = NULL;
	transferQueueCreateInfo->flags = 0;
	transferQueueCreateInfo->pQueuePriorities = globalQueuePriority;
	transferQueueCreateInfo->queueCount = 1;
	transferQueueCreateInfo->queueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

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
Renderer_DeviceQueues
VSR_DeviceQueuesSelectFromDevice(
	VkPhysicalDevice physicalDevice,
	VSR_Renderer* renderer
	)
{
	Renderer_DeviceQueues queues;

	///////////////////////////////////
	/// Get queue family properties ///
	///////////////////////////////////
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice,
											  &queueFamilyCount,
											  NULL);

	VkQueueFamilyProperties2* queueFamilyProperties2List = SDL_malloc(
		queueFamilyCount * sizeof(VkQueueFamilyProperties2) );

	for(size_t i = 0; i < queueFamilyCount; i++)
	{
		VkQueueFamilyProperties2* qFamily = &queueFamilyProperties2List[i];

		qFamily->sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
		qFamily->pNext = NULL;
	}

	vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice,
											  &queueFamilyCount,
											  queueFamilyProperties2List);

	//////////////////////////////////
	/// Select best queue families ///
	//////////////////////////////////
	VkBool32 hasGraphicsQ = VK_FALSE;
	VkBool32 hasComputeQ  = VK_FALSE;
	VkBool32 hasTransferQ = VK_FALSE;

	// TODO: test present support

	queues.computeCanPresent   = VK_FALSE;

	for( size_t i = 0; i < queueFamilyCount; i++)
	{
		VkQueueFamilyProperties properties =
									queueFamilyProperties2List[i].queueFamilyProperties;

		if(!hasGraphicsQ && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			queues.graphicsQueueFamilyIndex = i;
			queues.graphicsQueueIndex = 0;
			hasGraphicsQ = VK_TRUE;

			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice,
												 i,
												 renderer->subStructs->surface.surface,
												 &queues.graphicsCanPresent);
		}

		if(!hasComputeQ && properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			queues.computeQueueFamilyIndex = i;
			queues.computeQueueIndex = 0;
			hasComputeQ = VK_TRUE;

			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice,
												 i,
												 renderer->subStructs->surface.surface,
												 &queues.computeCanPresent);
		}

		if(!hasTransferQ && properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			queues.transferQueueFamilyIndex = i;
			queues.transferQueueIndex = 0;
			hasTransferQ = VK_TRUE;
		}
	}

	if(queues.computeQueueFamilyIndex == queues.graphicsQueueFamilyIndex)
	{
		queues.computeQueueIndex++;
	}

	if(queues.transferQueueFamilyIndex == queues.computeQueueFamilyIndex)
	{
		queues.transferQueueIndex++;
	}

	if(queues.transferQueueFamilyIndex == queues.graphicsQueueFamilyIndex)
	{
		queues.transferQueueIndex++;
	}

	// done with this now
	SDL_free((void*)queueFamilyProperties2List);

	return queues;
}





//==============================================================================
// VSR_DeviceQueuesCreate
//------------------------------------------------------------------------------
SDL_bool
VSR_DeviceQueuesCreate(
	VSR_Renderer* renderer,
	Renderer_CreateInfoSubStructs* subStructs)
{
	// now we can assume that our chosen device is final!

	// so the refactor has a few issues....
	VkDeviceQueueCreateInfo* graphicsQueueCreateInfo =
		&subStructs->deviceQueuesCreateInfo.queueCreateInfoList[
			subStructs->deviceQueuesCreateInfo.graphicsQueueCreateInfoIndex];

	graphicsQueueCreateInfo->queueFamilyIndex =
			renderer->subStructs->deviceQueues.graphicsQueueFamilyIndex;


	VkDeviceQueueCreateInfo* computeQueueCreateInfo =
		&subStructs->deviceQueuesCreateInfo.queueCreateInfoList[
			subStructs->deviceQueuesCreateInfo.computeQueueCreateInfoIndex];

	computeQueueCreateInfo->queueFamilyIndex =
		renderer->subStructs->deviceQueues.computeQueueFamilyIndex;

	VkDeviceQueueCreateInfo* transferQueueCreateInfo =
		&subStructs->deviceQueuesCreateInfo.queueCreateInfoList[
			subStructs->deviceQueuesCreateInfo.transferQueueCreateInfoIndex];

	transferQueueCreateInfo->queueFamilyIndex =
		renderer->subStructs->deviceQueues.transferQueueFamilyIndex;

// FIXME: this is a total hack to avoid writing a set implantation
// TODO: come up with a way of doing this that doesnt involve Writing a set
	graphicsQueueCreateInfo->queueCount = 3;
	subStructs->logicalDeviceCreateInfo.createInfo.queueCreateInfoCount = 1;

SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}