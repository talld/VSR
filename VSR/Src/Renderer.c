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
	
	VkPhysicalDeviceFeatures2    physicalDeviceFeatures2;
	
	VkDeviceQueueCreateInfo      queueCreateInfoList[3];
	size_t                       transferQueueCreateInfoIndex;
	size_t                       graphicsQueueCreateInfoIndex;
	size_t                       computeQueueCreateInfoIndex;

	VkDeviceCreateInfo           deviceCreateInfo;
	
	VkSwapchainCreateInfoKHR     swapchainCreateInfo;
	
	VkPipelineLayoutCreateInfo   graphicsPipelineLayoutCreateInfo;
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
	
	VkPipelineLayoutCreateInfo   computePipelineLayoutCreateInfo;
	VkComputePipelineCreateInfo  computePipelineCreateInfo;
	
};





typedef struct VSR_RendererCreateInfo VSR_RendererCreateInfo;
struct VSR_RendererCreateInfo
{
	SDL_Window*   SDLWindow;
	
	SDL_bool   geometryShaderRequested;
	SDL_bool   tessellationShaderRequested;
	
	VSR_RendererCreateInfoVkStructs*   vkStructs;
};





//==============================================================================
// VSR_PopulateInstanceCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_PopulateInstanceCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoVkStructs* vkStructs)
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





//==============================================================================
// VSR_RendererGenerateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_RendererPopulateLogicalDeviceCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoVkStructs* vkStructs)
{
	VkDeviceCreateInfo* deviceCreateInfo = &vkStructs->deviceCreateInfo;
	
	//////////////////////////////////////////////////////////
	/// Fill the features list with any features requested ///
	//////////////////////////////////////////////////////////
	
	if(createInfo->geometryShaderRequested)
	{
		vkStructs->physicalDeviceFeatures2.features.geometryShader = VK_TRUE;
	}
	
	if(createInfo->tessellationShaderRequested)
	{
		vkStructs->physicalDeviceFeatures2.features.tessellationShader = VK_TRUE;
	}
	
	////////////////////////////////////////////////////////////
	/// Fill deviceCreateInfo with what we can at this stage ///
	////////////////////////////////////////////////////////////
	
	/////////////////////
	/// Setup queues  ///
	/////////////////////
	
	// we're only going to use 3 queues to make things simple
	// 1 transfer queue
	// 2 graphics queue
	// 3 compute queue
	// managing queuePriority individually may be to complex, fix it for now
	static const float globalQueuePriority[3] = {1.0f,1.0f,1.0f};
	
	
	VkDeviceQueueCreateInfo* graphicsQueueCreateInfo =
		&vkStructs->queueCreateInfoList[vkStructs->graphicsQueueCreateInfoIndex];
	
	graphicsQueueCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphicsQueueCreateInfo->pNext = NULL;
	graphicsQueueCreateInfo->flags = 0;
	graphicsQueueCreateInfo->pQueuePriorities = globalQueuePriority;
	graphicsQueueCreateInfo->queueCount = 1;
	graphicsQueueCreateInfo->queueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;


	VkDeviceQueueCreateInfo* computeQueueCreateInfo =
		&vkStructs->queueCreateInfoList[vkStructs->computeQueueCreateInfoIndex];
	
	computeQueueCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	computeQueueCreateInfo->pNext = NULL;
	computeQueueCreateInfo->flags = 0;
	computeQueueCreateInfo->pQueuePriorities = globalQueuePriority;
	computeQueueCreateInfo->queueCount = 1;
	computeQueueCreateInfo->queueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	
	
	VkDeviceQueueCreateInfo* transferQueueCreateInfo =
		&vkStructs->queueCreateInfoList[vkStructs->transferQueueCreateInfoIndex];
	
	transferQueueCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	transferQueueCreateInfo->pNext = NULL;
	transferQueueCreateInfo->flags = 0;
	transferQueueCreateInfo->pQueuePriorities = globalQueuePriority;
	transferQueueCreateInfo->queueCount = 1;
	transferQueueCreateInfo->queueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	
	
	///////////////////////////////////////////
	/// Load queues into device create info ///
	///////////////////////////////////////////
	
	deviceCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo->flags = 0;
	deviceCreateInfo->pNext = NULL;
	
	deviceCreateInfo->pEnabledFeatures =
		&vkStructs->physicalDeviceFeatures2.features;
	
	deviceCreateInfo->ppEnabledExtensionNames = NULL;
	deviceCreateInfo->enabledExtensionCount = 0;
	deviceCreateInfo->enabledLayerCount = 0;
	deviceCreateInfo->ppEnabledLayerNames = NULL;
	
	// load the queue list
	deviceCreateInfo->pQueueCreateInfos = vkStructs->queueCreateInfoList;
	deviceCreateInfo->queueCreateInfoCount = 3;


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
// VSR_RendererPopulateGraphicsPipelineCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_RendererPopulateSwapchainCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoVkStructs* vkStructs)
{
	
	VkSwapchainCreateInfoKHR* swapchainCreateInfo  =
		&vkStructs->swapchainCreateInfo;
	
	
	swapchainCreateInfo->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo->pNext = NULL;
	swapchainCreateInfo->flags = 0;
	
	
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
// VSR_RendererPopulateGraphicsPipelineCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_RendererPopulateGraphicsPipelineCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoVkStructs* vkStructs)
{
	//////////////////////////
	/// Layout Create Info ///
	//////////////////////////
	VkPipelineLayoutCreateInfo* layoutCreateInfo =
		&vkStructs->graphicsPipelineLayoutCreateInfo;
	
	layoutCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo->pNext = NULL;
	layoutCreateInfo->flags = 0;
	
	
	////////////////////////////
	/// Pipeline Create Info ///
	////////////////////////////
	VkGraphicsPipelineCreateInfo* pipelineCreateInfo =
		&vkStructs->graphicsPipelineCreateInfo;
	
	pipelineCreateInfo->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo->pNext = NULL;
	pipelineCreateInfo->flags = 0;
	
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
VSR_RendererGenerateCreateInfo(
	SDL_Window* window,
	VSR_CreateInfoFlags flags)
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
	
	// assign constants
	
	// remove const to write these values outside of initiation
	createInfo->vkStructs->graphicsQueueCreateInfoIndex = 0;
	createInfo->vkStructs->transferQueueCreateInfoIndex = 1;
	createInfo->vkStructs->computeQueueCreateInfoIndex  = 2;
	
	VSR_RendererCreateInfoVkStructs* vkStructs = createInfo->vkStructs;


	////////////////////////////
	/// Process passed flags ///
	////////////////////////////
	if(flags & VSR_CREATE_INFO_GEOMETRY_SHADER)
	{
		createInfo->geometryShaderRequested = SDL_TRUE;
	}
	
	if(flags & VSR_CREATE_INFO_TESSELATION_SHADER)
	{
		createInfo->tessellationShaderRequested = SDL_TRUE;
	}

	
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
	createInfo->SDLWindow = window;
	
	////////////////////////////////////////////////////////////////////////////
	/// populate vk create info structs as much as can be done at the moment ///
	////////////////////////////////////////////////////////////////////////////
	VSR_PopulateInstanceCreateInfo(createInfo, vkStructs);
	VSR_RendererPopulateLogicalDeviceCreateInfo(createInfo, vkStructs);
	
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
VSR_RendererFreeCreateInfo(
	VSR_RendererCreateInfo* rendererCreateInfo)
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
	
	VkSurfaceKHR surface;

	uint32_t   transferQueueIndex;
	uint32_t   graphicsQueueIndex;
	uint32_t   computeQueueIndex;

	uint32_t   transferQueueFamilyIndex;
	uint32_t   graphicsQueueFamilyIndex;
	uint32_t   computeQueueFamilyIndex;

	VkBool32   graphicsCanPresent;
	VkBool32   computeCanPresent;

	VkQueue    graphicsQueue;
	VkQueue    presentQueue;
	VkQueue    transferQueue;
	VkQueue    computeQueue;
	
	VkDevice   logicalDevice;
};

typedef struct VSR_Renderer VSR_Renderer;
struct VSR_Renderer
{
	SDL_Window* SDLWindow;
	VSR_RendererVkStructs* vkStructs;
};





//==============================================================================
// VSR_CreateInstance
//------------------------------------------------------------------------------
SDL_bool
VSR_CreateInstance(
	VSR_Renderer* renderer,
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
	/// Get available layers     ///
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
		SDL_malloc( layerCount * sizeof(VkLayerProperties) );
	
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
	
	VkSurfaceKHR surfaceKhr;
	SDL_bool SDLErr;
	SDLErr = SDL_Vulkan_CreateSurface(renderer->SDLWindow,
									  instance,
									  &surfaceKhr);
	
	if(SDLErr != SDL_TRUE)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create window surface: %s",
		        SDL_GetError());
		
		VSR_SetErr(errMsg);
		goto FAIL;
	}
	
	renderer->vkStructs->surface = surfaceKhr;
	
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
VSR_SelectPhysicalDevice(
	VSR_Renderer* renderer,
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
		
		VkBool32 canVertex  = deviceVulkan11Properties.subgroupSupportedStages
		                     & VK_SHADER_STAGE_VERTEX_BIT;
		
		if(!canVertex) // if we can't actually draw vertexes the device
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
	
	VSR_LOG("Selected device: %s",
			chosenDeviceProperties.properties.deviceName);
	
	free((void*)physicalDeviceList);
	
	
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
VSR_SelectPhysicalDeviceQueues(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoVkStructs* vkStructs)
{
	// TODO: this function should be used to check if a given device supports
	// a particular surface and has a given queue
	
	// FIXME: refactor this function to act as a getQueues for ANY device
	
	/////////////////////////////
	/// easier to use aliases ///
	/////////////////////////////
	VkPhysicalDevice* pPhysicalDevice = &renderer->vkStructs->physicalDevice;
	
	VkDeviceQueueCreateInfo* graphicsQueueCreateInfo =
		&vkStructs->
			queueCreateInfoList[vkStructs->graphicsQueueCreateInfoIndex];
	
	VkDeviceQueueCreateInfo* computeQueueCreateInfo =
		&vkStructs->
			queueCreateInfoList[vkStructs->computeQueueCreateInfoIndex];
	
	VkDeviceQueueCreateInfo* transferQueueCreateInfo =
		&vkStructs->
			queueCreateInfoList[vkStructs->transferQueueCreateInfoIndex];
	
	uint32_t* pGraphicsQIndex  = &graphicsQueueCreateInfo->queueFamilyIndex;
	uint32_t* pComputeQIndex   = &computeQueueCreateInfo->queueFamilyIndex;
	uint32_t* pTransferQIndex  = &transferQueueCreateInfo->queueFamilyIndex;
	
	///////////////////////////////////
	/// Get queue family properties ///
	///////////////////////////////////
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties2(*pPhysicalDevice,
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
	
	vkGetPhysicalDeviceQueueFamilyProperties2(*pPhysicalDevice,
	                                          &queueFamilyCount,
	                                          queueFamilyProperties2List);
	
	//////////////////////////////////
	/// Select best queue families ///
	//////////////////////////////////
	VkBool32 hasGraphicsQ = VK_FALSE;
	VkBool32 hasComputeQ  = VK_FALSE;
	VkBool32 hasTransferQ = VK_FALSE;
	
	// TODO: test present support
	renderer->vkStructs->graphicsCanPresent  = VK_FALSE;
	renderer->vkStructs->computeCanPresent   = VK_FALSE;
	
	for( size_t i = 0; i < queueFamilyCount; i++)
	{
		VkQueueFamilyProperties properties =
			queueFamilyProperties2List[i].queueFamilyProperties;

		if(!hasGraphicsQ && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			*pGraphicsQIndex = i;
			hasGraphicsQ = VK_TRUE;
		}
		
		if(!hasComputeQ && properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			*pComputeQIndex = i;
			hasComputeQ = VK_TRUE;
		}
		
		if(!hasTransferQ && properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			*pTransferQIndex = i;
			hasTransferQ = VK_TRUE;
		}
	}
	
	/////////////////////////////////////////////
	/// Merge queue create infos if required ///
	/////////////////////////////////////////////
	
	// TODO: FIX THIS
	// FIXME: wont work if compute isn't merged but transfer is
	

	VkBool32  computeWasMerged = VK_FALSE;
		
	if( *pComputeQIndex == *pTransferQIndex)
	{
		transferQueueCreateInfo->queueCount +=
			computeQueueCreateInfo->queueCount;
			
		computeWasMerged = VK_TRUE;
	}
		
	if(!computeWasMerged && *pComputeQIndex == *pGraphicsQIndex)
	{
		graphicsQueueCreateInfo->queueCount +=
			computeQueueCreateInfo->queueCount;
			
		computeWasMerged = VK_TRUE;
	}
		
	if(computeWasMerged)
	{
		vkStructs->deviceCreateInfo.queueCreateInfoCount--;
	}

	
	if( *pTransferQIndex == *pGraphicsQIndex)
	{
		graphicsQueueCreateInfo->queueCount +=
			transferQueueCreateInfo->queueCount;
		
		vkStructs->deviceCreateInfo.queueCreateInfoCount--;
	}
	
	///////////////////////////////////////////////////////////////////
	/// Floor queue counts if they overrun the limits of the family ///
	///////////////////////////////////////////////////////////////////
	if(queueFamilyProperties2List[*pGraphicsQIndex]
	.queueFamilyProperties.queueCount < graphicsQueueCreateInfo->queueCount)
	{
		graphicsQueueCreateInfo->queueCount = queueFamilyProperties2List[*pGraphicsQIndex]
			.queueFamilyProperties.queueCount;
	}
	
	if(queueFamilyProperties2List[*pTransferQIndex]
		   .queueFamilyProperties.queueCount < transferQueueCreateInfo->queueCount)
	{
		transferQueueCreateInfo->queueCount = queueFamilyProperties2List[*pTransferQIndex]
			.queueFamilyProperties.queueCount;
	}
	
	if(queueFamilyProperties2List[*pComputeQIndex]
		   .queueFamilyProperties.queueCount < computeQueueCreateInfo->queueCount)
	{
		computeQueueCreateInfo->queueCount = queueFamilyProperties2List[*pComputeQIndex]
			.queueFamilyProperties.queueCount;
	}
	
	// done with this now
	free((void*)queueFamilyProperties2List);
	
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
VSR_CreateLogicalDevice(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoVkStructs* vkStructs)
{
	VkResult err;
	VkDevice logicalDevice;

	/////////////////////////////
	/// Create logical device ///
	/////////////////////////////
	err = vkCreateDevice(renderer->vkStructs->physicalDevice,
				   &vkStructs->deviceCreateInfo,
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
	
	renderer->vkStructs->logicalDevice = logicalDevice;

	// FIXME: this is broken, handle queues better!

	vkGetDeviceQueue(renderer->vkStructs->logicalDevice,
					 renderer->vkStructs->graphicsQueueFamilyIndex,
					 renderer->vkStructs->graphicsQueueIndex,
					 &renderer->vkStructs->graphicsQueue);

	// FIXME: this won't always be the case!
	vkGetDeviceQueue(renderer->vkStructs->logicalDevice,
					 renderer->vkStructs->graphicsQueueFamilyIndex,
					 renderer->vkStructs->graphicsQueueIndex,
					 &renderer->vkStructs->presentQueue);

	vkGetDeviceQueue(renderer->vkStructs->logicalDevice,
					 renderer->vkStructs->transferQueueFamilyIndex,
					 renderer->vkStructs->transferQueueIndex,
					 &renderer->vkStructs->transferQueue);


	vkGetDeviceQueue(renderer->vkStructs->logicalDevice,
					 renderer->vkStructs->computeQueueFamilyIndex,
					 renderer->vkStructs->computeQueueIndex,
					 &renderer->vkStructs->computeQueue);
	
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
// VSR_CreateGraphicsPipeline
//------------------------------------------------------------------------------
SDL_bool
VSR_CreateGraphicsPipeline(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoVkStructs* vkStructs)
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
// VSR_CreateRenderer
//------------------------------------------------------------------------------
VSR_Renderer*
VSR_CreateRenderer(
	VSR_RendererCreateInfo* rendererCreateInfo)
{
	VSR_Renderer* renderer = SDL_calloc(1, sizeof(VSR_Renderer));
	renderer->vkStructs = SDL_calloc(1, sizeof(VSR_RendererVkStructs));
	
	//////////////////////////////////
	/// pass info to new structure ///
	//////////////////////////////////
	renderer->SDLWindow = rendererCreateInfo->SDLWindow;
	
	VSR_CreateInstance(renderer, rendererCreateInfo->vkStructs);
	VSR_SelectPhysicalDevice(renderer, rendererCreateInfo->vkStructs);
	VSR_SelectPhysicalDeviceQueues(renderer, rendererCreateInfo->vkStructs);
	VSR_CreateLogicalDevice(renderer, rendererCreateInfo->vkStructs);
	VSR_CreateGraphicsPipeline(renderer, rendererCreateInfo->vkStructs);
	
	return renderer;
}




//==============================================================================
// VSR_FreeRenderer
//------------------------------------------------------------------------------
void
VSR_FreeRenderer(
	VSR_Renderer* renderer)
{
	////////////////////////////////////////
	/// Destroy VkStructs Vulkan objects ///
	////////////////////////////////////////
	
	vkDestroyDevice(renderer->vkStructs->logicalDevice,
					VSR_GetAllocator());
	
	vkDestroySurfaceKHR(renderer->vkStructs->instance,
							renderer->vkStructs->surface,
							VSR_GetAllocator());
	
	vkDestroyInstance(renderer->vkStructs->instance,
					  VSR_GetAllocator());
	
	/////////////////////////////////
	/// Free renderer's VKStructs ///
	/////////////////////////////////
	free((void*)renderer->vkStructs);
	
	/////////////////////
	/// Free renderer ///
	/////////////////////
	free((void*)renderer);
}