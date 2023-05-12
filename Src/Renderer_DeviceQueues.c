#include "Renderer_DeviceQueues.h"

#include "VSR_Renderer.h"

SDL_bool VSR_DeviceQueuesPopulateCreateInfo(
	VSR_RendererCreateInfo* rendererCreateInfo)
{


	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}

void findFistQueue(
	VkPhysicalDevice physicalDevice,
	VkSurfaceKHR surface,
	uint32_t index,
	VkQueueFamilyProperties* properties,
	VkQueueFlagBits flagBits,
	VkBool32* canPresent,
	VkBool32* valid)
{
	if(properties[index].queueFlags & flagBits)
	{
		if(valid != NULL)
		{
			*valid = VK_TRUE;
		}

		if(canPresent != NULL)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice,
			                                     index,
			                                     surface,
			                                     canPresent);
		}
	}
}




//==============================================================================
// VSR_SelectPhysicalDevice
//------------------------------------------------------------------------------
Renderer_DeviceQueues
VSR_DeviceQueuesSelectFromDevice(
	VkPhysicalDevice physicalDevice,
	VSR_Renderer* renderer)
{
	Renderer_DeviceQueues queues = (Renderer_DeviceQueues){0};

	//////////////////////////////////
	/// Select best queue families ///
	//////////////////////////////////
	VkBool32 hasGraphicsQ = VK_FALSE;
	VkBool32 hasComputeQ  = VK_FALSE;
	VkBool32 hasTransferQ = VK_FALSE;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		physicalDevice,
		&queueFamilyCount,
		NULL
	);

	VkQueueFamilyProperties* pQueueFamilyProperties =
		SDL_malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(
		physicalDevice,
		&queueFamilyCount,
		pQueueFamilyProperties
	);

	for( uint32_t i = 0; i < queueFamilyCount; i++)
	{

		if(!hasGraphicsQ)
		{
			findFistQueue(physicalDevice,
			              renderer->surface.surface,
			              i,
			              pQueueFamilyProperties,
			              VK_QUEUE_GRAPHICS_BIT,
			              &queues.QCanPresent[kGraphicsQueueIndex],
			              &hasGraphicsQ
			);

			queues.QFamilies[kGraphicsQueueIndex] = i;
		}

		if(!hasComputeQ)
		{
			findFistQueue(physicalDevice,
			              renderer->surface.surface,
			              i,
			              pQueueFamilyProperties,
			              VK_QUEUE_COMPUTE_BIT,
			              &queues.QCanPresent[kComputeQueueIndex],
			              &hasComputeQ
			);

				queues.QFamilies[kComputeQueueIndex] = i;
		}

		if(!hasTransferQ)
		{
			findFistQueue(physicalDevice,
			              renderer->surface.surface,
			              i,
			              pQueueFamilyProperties,
			              VK_QUEUE_COMPUTE_BIT,
			              &queues.QCanPresent[kTransferQueueIndex],
			              &hasTransferQ
			);

			queues.QFamilies[kTransferQueueIndex] = i;
		}
	}


	return queues;
}





//==============================================================================
// VSR_DeviceQueuesCreate
//------------------------------------------------------------------------------
SDL_bool
VSR_DeviceQueuesCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo)
{

SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}