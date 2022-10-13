#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include "VSR_error.h"

// Temp allocate function
// TODO: replace with function in VSR_mem.h ( or whatever )
const VkAllocationCallbacks*
VSR_GetAllocator()
{
	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
/// Renderer createInfo                                                      ///
////////////////////////////////////////////////////////////////////////////////
typedef struct VSR_RendererCreateInfoVkStructs VSR_RendererCreateInfoVkStructs;
struct VSR_RendererCreateInfoVkStructs
{
	VkApplicationInfo            applicationInfo;
	VkInstanceCreateInfo         instanceCreateInfo;
	
	VkDeviceCreateInfo           deviceCreateInfo;
	
	VkComputePipelineCreateInfo  computePipelineCreateInfo;
	
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
	
};





typedef struct VSR_RendererCreateInfo VSR_RendererCreateInfo;
struct VSR_RendererCreateInfo
{
	VSR_RendererCreateInfoVkStructs* vkStructs;
};





//==============================================================================
// VSR_PopulateInstanceCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_PopulateInstanceCreateInfo(VSR_RendererCreateInfoVkStructs* vkStructs)
{
	VkApplicationInfo* applicationInfo = &vkStructs->applicationInfo;
	VkInstanceCreateInfo* instanceCreateInfo = &vkStructs->instanceCreateInfo;
	
	applicationInfo->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo->apiVersion = VK_API_VERSION_1_3;
	applicationInfo->engineVersion = 0;
	applicationInfo->pEngineName = "VSR";

	

	
	instanceCreateInfo->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo->pApplicationInfo = applicationInfo;
	
	#ifdef VSR_DEBUG
	{
		static const char* layerName =  "VK_LAYER_KHRONOS_validation";
		instanceCreateInfo->ppEnabledLayerNames = &layerName;
		instanceCreateInfo->enabledLayerCount = 1;
	}
	#endif
	
	SUCCESS:
	{
		return SDL_TRUE;
	}
	
	FAIL:
	{
		return SDL_FALSE;
	}
}


SDL_bool
VSR_RendererPopulateLogicalDeviceCreateInfo(VSR_RendererCreateInfoVkStructs* vkStructs)
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



//==============================================================================
// VSR_RendererGenerateCreateInfo
//------------------------------------------------------------------------------
VSR_RendererCreateInfo*
VSR_RendererGenerateCreateInfo(SDL_Window* window, VSR_CreateInfoFlags flags)
{
	// TODO: at a later stage function call for vulkan should be gotten by
	// pulling functions pointers from these functions
	// vkGetInstanceProcAddr = SDL_Vulkan_GetVkGetInstanceProcAddr();
	// fn = vkGetInstanceProcAddr(VkInstance instance, const char* name);
	// fn(args);
	//
	// this is to ensure that SDL and VSR are using the same version of
	// vulkan and they're function calls will always do the same thing
	
	////////////////////////
	/// allocate structs ///
	////////////////////////
	size_t createInfoSize = sizeof(VSR_RendererCreateInfo);
	VSR_RendererCreateInfo* createInfo =  calloc(1, createInfoSize);
	
	size_t vkStructSize = sizeof(VSR_RendererCreateInfoVkStructs);
	createInfo->vkStructs = calloc(1, vkStructSize);
	
	VSR_RendererCreateInfoVkStructs* vkStructs = createInfo->vkStructs;
	
	////////////////////////////////////////////////
	/// query SDL for the extensions it requires ///
	////////////////////////////////////////////////
	SDL_bool result;
	unsigned int SDLExtCount;
	result = SDL_Vulkan_GetInstanceExtensions(window, &SDLExtCount, NULL);
	
	if(result == SDL_FALSE)
	{
		VSR_SetErr("Failed to enumerate SDL extension count");
		goto FAIL;
	}
	
	// TODO: not malloc this
	const char** ppSDLExtNames = malloc( SDLExtCount * sizeof(char*) );
	
	result = SDL_Vulkan_GetInstanceExtensions(window,
											  &SDLExtCount,
											  ppSDLExtNames);
	
	if(result == SDL_FALSE)
	{
		VSR_SetErr("Failed to retrieve SDL extensions");
		goto FAIL;
	}
	
	// keep track of info in vkStructs, we could always add to it later;
	vkStructs->instanceCreateInfo.enabledExtensionCount = SDLExtCount;
	vkStructs->instanceCreateInfo.ppEnabledExtensionNames = ppSDLExtNames;
	
	////////////////////////////////////////////////////////////////////////////
	/// populate vk create info structs as much as can be done at the moment ///
	////////////////////////////////////////////////////////////////////////////
	VSR_PopulateInstanceCreateInfo(vkStructs);
	
	SUCCESS:
	{
		return createInfo;
	}
	
	FAIL:
	{
		return NULL;
	}
}





//==============================================================================
// VSR_RendererFreeCreateInfo
//------------------------------------------------------------------------------
void
VSR_RendererFreeCreateInfo(VSR_RendererCreateInfo* rendererCreateInfo)
{
	VSR_RendererCreateInfoVkStructs* vkStructs = rendererCreateInfo->vkStructs;
	
	// in an ideal world all of this would not exist
	// all the memory needed should be part of the VSR_RendererCreateInfo struct
	// work should be done to minimise malloc wherever possible
	
	///////////////////////////////////////////
	/// Free vkStructs create info pointers ///
	///////////////////////////////////////////
	// this can be removed when the malloc is
	free((void*) vkStructs->instanceCreateInfo.ppEnabledExtensionNames);
	
	free((void*)vkStructs);
	
	///////////////////////////////////
	/// free the rendererCreateInfo ///
	///////////////////////////////////
	free((void*)rendererCreateInfo);
}





////////////////////////////////////////////////////////////////////////////////
/// Renderer proper                                                          ///
////////////////////////////////////////////////////////////////////////////////
typedef struct VSR_RendererVkStructs VSR_RendererVkStructs;
struct VSR_RendererVkStructs
{
	VkInstance         instance;
	
	VkPhysicalDevice                      physicalDevice;
	VkPhysicalDeviceProperties2           physicalDeviceProperties;
	VkPhysicalDeviceVulkan11Properties    physicalDeviceVulkan11Properties;
	VkPhysicalDeviceVulkan12Properties    physicalDeviceVulkan12Properties;
	VkPhysicalDeviceVulkan13Properties    physicalDeviceVulkan13Properties;
	
	VkDevice           logicalDevice;
};

typedef struct VSR_Renderer VSR_Renderer;
struct VSR_Renderer
{
	VSR_RendererVkStructs* vkStructs;
};





//==============================================================================
// VSR_CreateInstance
//------------------------------------------------------------------------------
SDL_bool
VSR_CreateInstance(VSR_Renderer* renderer,
                   VSR_RendererCreateInfoVkStructs* vkStructs)
{
	VkResult err;
	VkInstanceCreateInfo* instanceCreateInfo = &vkStructs->instanceCreateInfo;
	
	////////////////////////////////
	/// Get available extensions ///
	////////////////////////////////
	uint32_t propertyCount;
	err = vkEnumerateInstanceExtensionProperties(NULL,
												 &propertyCount,
												 NULL);
	
	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to retrieve instance extensions: %s",
		        VSR_VkErrorToString(err));
		
		VSR_SetErr(errMsg);
		goto FAIL;
	}
	
	VkExtensionProperties* availableProperties =
		SDL_malloc( propertyCount * sizeof(VkExtensionProperties) );
	
	err = vkEnumerateInstanceExtensionProperties(NULL,
												 &propertyCount,
												 availableProperties);
	
	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to retrieve instance extensions: %s",
		        VSR_VkErrorToString(err));
		
		VSR_SetErr(errMsg);
		goto FAIL;
	}
	
	///////////////////////////////////////////////////////
	/// Check that we have all the extensions SDL needs ///
	///////////////////////////////////////////////////////
	size_t foundExtensions = 0;
	size_t requiredExtensions = instanceCreateInfo->enabledExtensionCount;
	for(size_t i = 0; i < propertyCount; i++)
	{
		VSR_LOG("FOUND_EXTENSION: %s",availableProperties[i].extensionName);
		for(size_t j = 0; j < requiredExtensions; j++)
		{
			if( strcmp(availableProperties[i].extensionName,
			           instanceCreateInfo->ppEnabledExtensionNames[j]) == 0 )
			{
				VSR_LOG("MATCHES: %s",
						instanceCreateInfo->ppEnabledExtensionNames[j]);
				
				foundExtensions++;
			}
		}
	}
	// list of availableProperties no longer required
	free((void*)availableProperties);
	
	VSR_LOG("REQUIRED EXTENSIONS: %lu, FOUND %lu", requiredExtensions, foundExtensions);
	if(foundExtensions != requiredExtensions)
	{
		VSR_SetErr("Could not find all required extensions");
		goto FAIL;
	}
	
	////////////////////////////////
	/// Get available layers ///
	////////////////////////////////
	uint32_t layerCount;
	err = vkEnumerateInstanceLayerProperties(&layerCount, NULL);
	
	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to retrieve instance layers: %s",
		        VSR_VkErrorToString(err));
		
		VSR_SetErr(errMsg);
		goto FAIL;
	}
	
	VkLayerProperties * availableLayers =
		SDL_malloc( propertyCount * sizeof(VkExtensionProperties) );
	
	err = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
	
	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to retrieve instance layers: %s",
		        VSR_VkErrorToString(err));
		
		VSR_SetErr(errMsg);
		goto FAIL;
	}
	
	///////////////////////////////////////////////////////
	/// Check that we have all the layers SDL needs ///
	///////////////////////////////////////////////////////
	size_t foundLayers = 0;
	size_t requiredLayers = instanceCreateInfo->enabledLayerCount;
	for(size_t i = 0; i < layerCount; i++)
	{
		VSR_LOG("FOUND_LAYER: %s",availableLayers[i].layerName);
		for(size_t j = 0; j < requiredLayers; j++)
		{
			if( strcmp(availableLayers[i].layerName,
			           instanceCreateInfo->ppEnabledLayerNames[j]) == 0 )
			{
				VSR_LOG("MATCHES: %s",
				        instanceCreateInfo->ppEnabledLayerNames[j]);
				
				foundLayers++;
			}
		}
	}
	// list of availableProperties no longer required
	free((void*)availableLayers);
	
	VSR_LOG("REQUIRED LAYERS: %lu, FOUND %lu", requiredLayers, foundLayers);
	if(foundLayers != requiredLayers)
	{
		VSR_SetErr("Could not find all required layers");
		goto FAIL;
	}
	
	
	
	VkInstance instance;
	err = vkCreateInstance(&vkStructs->instanceCreateInfo,
						   VSR_GetAllocator(),
						   &instance);
	
	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create instance: %s",
		        VSR_VkErrorToString(err));
		
		VSR_SetErr(errMsg);
		goto FAIL;
	}
	
	renderer->vkStructs->instance = instance;
	
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
VSR_SelectPhysicalDevice(VSR_Renderer* renderer,
						 VSR_RendererCreateInfoVkStructs* vkStructs)
{
	VkResult err;
	///////////////////////////////////////
	/// grab available physical devices ///
	///////////////////////////////////////
	uint32_t deviceCount = 0;
	
	err = vkEnumeratePhysicalDevices(renderer->vkStructs->instance,
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
	
	err = vkEnumeratePhysicalDevices(renderer->vkStructs->instance,
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
	VkPhysicalDevice chosenDevice;
	size_t chosenDeviceScore;
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
		
		size_t deviceScore = 0;
		
		// measure card by its max SINGLE alloc
		// ( simpler then trying to figure out vRam from the list of heaps )
		deviceScore += deviceVulkan11Properties.maxMemoryAllocationSize;
		
		// device score is basically a measure of vRam
		if(deviceProperties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{ // so if vRam is shared due to it being an integrated gpu
			deviceScore = deviceScore / 3; // assume 2/3s is going to be in use
		}
		
		if(deviceScore > chosenDeviceScore)
		{
			chosenDeviceScore = deviceScore;
			chosenDeviceProperties = deviceProperties;
			chosenDeviceVulkan11Properties = deviceVulkan11Properties;
			chosenDeviceVulkan12Properties = deviceVulkan12Properties;
			chosenDeviceVulkan13Properties = deviceVulkan13Properties;
			chosenDevice = testedDevice;
		}
	}
	
	////////////////////////////////////////////////
	/// Store the property structs for later use ///
	////////////////////////////////////////////////
	renderer->vkStructs->physicalDeviceProperties = chosenDeviceProperties;
	
	renderer->vkStructs->physicalDeviceVulkan11Properties =
		chosenDeviceVulkan11Properties;
	
	renderer->vkStructs->physicalDeviceVulkan12Properties =
		chosenDeviceVulkan12Properties;
	
	renderer->vkStructs->physicalDeviceVulkan13Properties =
		chosenDeviceVulkan13Properties;
	
	renderer->vkStructs->physicalDevice = chosenDevice;
	
	
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
VSR_CreateLogicalDevice(VSR_Renderer* renderer,
							VSR_RendererCreateInfoVkStructs* vkStructs)
{
	VkResult  err;
	
	err = vkCreateDevice(renderer->vkStructs->physicalDevice,
				   &vkStructs->deviceCreateInfo,
				   VSR_GetAllocator(),
				   &renderer->vkStructs->logicalDevice);
	
	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create logical device: %s",
		        VSR_VkErrorToString(err));
		
		VSR_SetErr(errMsg);
		goto FAIL;
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


VSR_Renderer*
VSR_CreateRenderer(VSR_RendererCreateInfo* rendererCreateInfo)
{
	VSR_Renderer* renderer = SDL_calloc(1, sizeof(VSR_Renderer));
	renderer->vkStructs = SDL_calloc(1, sizeof(VSR_RendererVkStructs));
	
	VSR_CreateInstance(renderer, rendererCreateInfo->vkStructs);
	VSR_SelectPhysicalDevice(renderer, rendererCreateInfo->vkStructs);
	VSR_CreateLogicalDevice(renderer, rendererCreateInfo->vkStructs);
	
	return renderer;
}

void
VSR_FreeRenderer(VSR_Renderer* renderer)
{
	////////////////////////////////////////
	/// Destroy VkStructs Vulkan objects ///
	////////////////////////////////////////
	vkDestroyDevice(renderer->vkStructs->logicalDevice, VSR_GetAllocator());
	vkDestroyInstance(renderer->vkStructs->instance, VSR_GetAllocator());
	
	/////////////////////////////////
	/// Free renderer's VKStructs ///
	/////////////////////////////////
	free((void*)renderer->vkStructs);
	
	/////////////////////
	/// Free renderer ///
	/////////////////////
	free((void*)renderer);
}