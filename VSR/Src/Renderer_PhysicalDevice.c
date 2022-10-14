#include "Renderer_PhysicalDevice.h"

#include "Renderer.h"
#include "Renderer_DeviceQueues.h"
#include "Renderer_Surface.h"
#include "VSR_error.h"

//==============================================================================
// VSR_SelectPhysicalDevice
//------------------------------------------------------------------------------
SDL_bool
VSR_PhysicalDeviceSelect(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* vkStructs)
{
	///////////////
	/// Aliases ///
	///////////////
	VkInstance* instance = &renderer->subStructs->instance.instance;

	VSR_PhysicalDevice* physicalDevice = &renderer->subStructs->physicalDevice;


	///////////////////////////////////////
	/// grab available physical devices ///
	///////////////////////////////////////
	VkResult err;
	uint32_t deviceCount = 0;

	err = vkEnumeratePhysicalDevices(*instance,
									 &deviceCount,
									 NULL);

	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to enumerate physical devices: %s",
				VSR_VkErrorToString(err));

		VSR_SetErr(errMsg);
		goto FAIL;
	}

	size_t physicalDeviceListSize = deviceCount * sizeof(VkPhysicalDevice);
	VkPhysicalDevice* physicalDeviceList = SDL_malloc(physicalDeviceListSize);

	err = vkEnumeratePhysicalDevices(*instance,
									 &deviceCount,
									 physicalDeviceList);

	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to enumerate physical devices: %s",
				VSR_VkErrorToString(err));

		VSR_SetErr(errMsg);
		goto FAIL;
	}

	///////////////////////////////////////
	/// Find the 'best' physical device ///
	///////////////////////////////////////
	VkPhysicalDeviceProperties2 chosenDeviceProperties; // using 2 for EXTs
	VkPhysicalDeviceVulkan11Properties chosenDeviceVulkan11Properties;
	VkPhysicalDeviceVulkan12Properties chosenDeviceVulkan12Properties;
	VkPhysicalDeviceVulkan13Properties chosenDeviceVulkan13Properties;
	VSR_DeviceQueues chosenDeviceQueues;
	VSR_Surface      chosenDeviceSurface;
	VkPhysicalDevice chosenDevice;
	size_t chosenDeviceScore = 0;
	for ( size_t i = 0; i < deviceCount; i++ )
	{
		//////////////////////////////
		/// Setup property structs ///
		//////////////////////////////
		// these may be largely unused, but it doesn't hurt to grab ALL the info
		VkPhysicalDeviceProperties2        deviceProperties;
		VkPhysicalDeviceVulkan11Properties deviceVulkan11Properties;
		VkPhysicalDeviceVulkan12Properties deviceVulkan12Properties;
		VkPhysicalDeviceVulkan13Properties deviceVulkan13Properties;

		deviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

		// not sure if these need sType to be populated but do it anyway
		deviceVulkan11Properties.sType =
			VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;

		deviceVulkan12Properties.sType =
			VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;

		deviceVulkan13Properties.sType =
			VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;


		// chain the structs so vulkan fills them all
		deviceProperties.pNext         = &deviceVulkan11Properties;
		deviceVulkan11Properties.pNext = &deviceVulkan12Properties;
		deviceVulkan12Properties.pNext = &deviceVulkan13Properties;
		deviceVulkan13Properties.pNext = NULL;
		///////////////////////
		/// Test the device ///
		///////////////////////
		VkPhysicalDevice testedDevice = physicalDeviceList[i];

		// this can't fail?
		vkGetPhysicalDeviceProperties2(testedDevice, &deviceProperties);

		VSR_LOG("Found device: %s", deviceProperties.properties.deviceName);

		size_t deviceScore = 0;

		// measure card by its max SINGLE alloc
		// ( simpler then trying to figure out vRam from the list of heaps )
		deviceScore += deviceVulkan11Properties.maxMemoryAllocationSize;

		// device score is basically a measure of vRam
		if(deviceProperties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{ // so if vRam is shared due to it being an integrated gpu
			deviceScore = deviceScore / 3; // assume 2/3s is going to be in use
		}

		VSR_DeviceQueues deviceQueues =
			VSR_DeviceQueuesSelectFromDevice(testedDevice,
											 renderer);

		VSR_Surface deviceSurface = VSR_SurfaceGetSurfaceFormatFromDevice(
			renderer,
			testedDevice);

		VkBool32 canVertex  = deviceVulkan11Properties.subgroupSupportedStages
							  & VK_SHADER_STAGE_VERTEX_BIT;

		VkBool32 canPresent =
			(deviceQueues.computeCanPresent || deviceQueues.graphicsCanPresent);

		if(!canVertex // if we can't actually draw vertexes the device
			|| !canPresent ) // or we can't find a way to present to the screen
		{
			deviceScore = 0; // don't use it
		}

		if(deviceScore > chosenDeviceScore)
		{
			chosenDeviceScore = deviceScore;
			chosenDeviceProperties = deviceProperties;
			chosenDeviceVulkan11Properties = deviceVulkan11Properties;
			chosenDeviceVulkan12Properties = deviceVulkan12Properties;
			chosenDeviceVulkan13Properties = deviceVulkan13Properties;
			chosenDeviceSurface = deviceSurface;
			chosenDeviceQueues = deviceQueues;
			chosenDevice = testedDevice;
		}
	}

	////////////////////////////////////////////////
	/// Store the property structs for later use ///
	////////////////////////////////////////////////
	physicalDevice->deviceProperties =
		chosenDeviceProperties;

	physicalDevice->deviceVulkan11Properties =
		chosenDeviceVulkan11Properties;

	physicalDevice->deviceVulkan12Properties =
		chosenDeviceVulkan12Properties;

	physicalDevice->deviceVulkan13Properties =
		chosenDeviceVulkan13Properties;

	renderer->subStructs->deviceQueues = chosenDeviceQueues;
	renderer->subStructs->surface = chosenDeviceSurface;

	physicalDevice->device =
		chosenDevice;

	VSR_LOG("Selected device: %s",
			chosenDeviceProperties.properties.deviceName);

	SDL_free((void*)physicalDeviceList);

	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}