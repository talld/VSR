#include "Renderer_Instance.h"

#include <SDL_vulkan.h>

#include "Renderer.h"
#include "VSR_error.h"

//==============================================================================
// VSR_PopulateInstanceCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_InstancePopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs)
{
	///////////////
	/// Aliases ///
	///////////////
	VSR_InstanceCreateInfo* instanceInfo = &subStructs->instanceCreateInfo;

	VkInstanceCreateInfo* instanceCreateInfo = &instanceInfo->createInfo;

	VkApplicationInfo* applicationInfo = &instanceInfo->applicationInfo;

	////////////////////////
	/// Application info ///
	////////////////////////
	applicationInfo->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo->apiVersion = VK_API_VERSION_1_3;
	applicationInfo->engineVersion = 0;
	applicationInfo->pEngineName = "VSR";

	instanceCreateInfo->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo->pApplicationInfo = applicationInfo;

	////////////////////////////////////////////////
	/// query SDL for the extensions it requires ///
	////////////////////////////////////////////////
	SDL_bool result;
	unsigned int SDLExtCount;
	result = SDL_Vulkan_GetInstanceExtensions(createInfo->SDLWindow,
											  &SDLExtCount,
											  NULL);

	if(result == SDL_FALSE)
	{
		VSR_SetErr("Failed to enumerate SDL extension count");
		goto FAIL;
	}

	// TODO: not malloc this
	const char** ppSDLExtNames = SDL_malloc( SDLExtCount * sizeof(char*) );

	result = SDL_Vulkan_GetInstanceExtensions(createInfo->SDLWindow,
											  &SDLExtCount,
											  ppSDLExtNames);

	if(result == SDL_FALSE)
	{
		VSR_SetErr("Failed to retrieve SDL extensions");
		goto FAIL;
	}

	// keep track of info in vkStructs, we could always add to it later;
	instanceCreateInfo->enabledExtensionCount   = SDLExtCount;
	instanceCreateInfo->ppEnabledExtensionNames = ppSDLExtNames;

	////////////////////////////////
	/// Populate required layers ///
	////////////////////////////////
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
// VSR_CreateInstance
//------------------------------------------------------------------------------
SDL_bool
VSR_InstanceCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs)
{
	///////////////
	/// Aliases ///
	///////////////
	VkInstanceCreateInfo* createInfo =
		&subStructs->instanceCreateInfo.createInfo;


	////////////////////////////////
	/// Get available extensions ///
	////////////////////////////////
	VkResult err;
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
	size_t requiredExtensions = createInfo->enabledExtensionCount;
	for(size_t i = 0; i < propertyCount; i++)
	{
		VSR_LOG("FOUND_EXTENSION: %s",availableProperties[i].extensionName);
		for(size_t j = 0; j < requiredExtensions; j++)
		{
			if( strcmp(availableProperties[i].extensionName,
					   createInfo->ppEnabledExtensionNames[j]) == 0 )
			{
				VSR_LOG("MATCHES: %s",
						createInfo->ppEnabledExtensionNames[j]);

				foundExtensions++;
			}
		}
	}
	// list of availableProperties no longer required
	SDL_free((void*)availableProperties);

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
	size_t requiredLayers = createInfo->enabledLayerCount;
	for(size_t i = 0; i < layerCount; i++)
	{
		VSR_LOG("FOUND_LAYER: %s",availableLayers[i].layerName);
		for(size_t j = 0; j < requiredLayers; j++)
		{
			if( strcmp(availableLayers[i].layerName,
					   createInfo->ppEnabledLayerNames[j]) == 0 )
			{
				VSR_LOG("MATCHES: %s",
						createInfo->ppEnabledLayerNames[j]);

				foundLayers++;
			}
		}
	}
	// list of availableProperties no longer required
	SDL_free((void*)availableLayers);

	VSR_LOG("REQUIRED LAYERS: %lu, FOUND %lu", requiredLayers, foundLayers);
	if(foundLayers != requiredLayers)
	{
		VSR_SetErr("Could not find all required layers");
		goto FAIL;
	}


	VkInstance instance;
	err = vkCreateInstance(createInfo,
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

	renderer->subStructs->instance.instance = instance;
	SDL_free((void*) createInfo->ppEnabledExtensionNames);

	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}