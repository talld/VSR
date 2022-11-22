#include <vulkan/vulkan.h>

#include "Renderer.h"
#include "VSR_error.h"

#include "vert.h"
#include "frag.h"




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
	VSR_RendererCreateInfo* createInfo =  SDL_calloc(1, createInfoSize);
	
	size_t subStructsSize = sizeof(VSR_RendererCreateInfoSubStructs);
	createInfo->subStructs = SDL_calloc(1, subStructsSize);

	createInfo->SDLWindow = window;

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

	////////////////////////////////////////////////////////////////////////////
	/// populate vk create info structs as much as can be done at the moment ///
	////////////////////////////////////////////////////////////////////////////
	VSR_InstancePopulateCreateInfo(createInfo, createInfo->subStructs);
	VSR_DeviceQueuesPopulateCreateInfo(createInfo, createInfo->subStructs);
    VSR_LogicalDevicePopulateCreateInfo(createInfo, createInfo->subStructs);
	VSR_SwapchainPopulateCreateInfo(createInfo, createInfo->subStructs);
	VSR_RendererPopulateRenderPassCreateInfo(createInfo, createInfo->subStructs);
	VSR_RendererPopulateGraphicsPipelineCreateInfo(createInfo, createInfo->subStructs);

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
	VSR_RendererCreateInfoSubStructs* subStructs =
		rendererCreateInfo->subStructs;
	
	// in an ideal world all of this would not exist
	// all the memory needed should be part of the VSR_RendererCreateInfo struct
	// work should be done to minimise malloc wherever possible
	
	///////////////////////////////////////////
	/// Free vkStructs create info pointers ///
	///////////////////////////////////////////
	SDL_free((void*)subStructs);
	
	///////////////////////////////////
	/// free the rendererCreateInfo ///
	///////////////////////////////////
	SDL_free((void*)rendererCreateInfo);
}





//==============================================================================
// VSR_CreateRenderer
//------------------------------------------------------------------------------
VSR_Renderer*
VSR_CreateRenderer(
	VSR_RendererCreateInfo* rendererCreateInfo)
{
	VSR_Renderer* renderer = SDL_calloc(1, sizeof(VSR_Renderer));
	renderer->subStructs = SDL_calloc(1, sizeof(VSR_RendererSubStructs));
	
	//////////////////////////////////
	/// pass info to new structure ///
	//////////////////////////////////
	renderer->SDLWindow = rendererCreateInfo->SDLWindow;
	
	VSR_InstanceCreate(renderer, rendererCreateInfo->subStructs);
	VSR_SurfaceCreate(renderer, rendererCreateInfo->subStructs);
	VSR_PhysicalDeviceSelect(renderer, rendererCreateInfo->subStructs);
	VSR_DeviceQueuesCreate(renderer, rendererCreateInfo->subStructs);
	VSR_LogicalDeviceCreate(renderer, rendererCreateInfo->subStructs);

	// TODO: move this to its own VSR_GraphicsPipeline struct
	VSR_SwapchainCreate(renderer, rendererCreateInfo->subStructs);

	renderer->subStructs->vertexShader =
		VSR_ShaderCreate(renderer, kVertexShaderBytesSize, vertexShaderBytes);

	renderer->subStructs->fragmentShader =
		VSR_ShaderCreate(renderer, kFragmentShaderBytesSize, fragmentShaderBytes);

	VSR_CreateRenderPass(renderer, rendererCreateInfo->subStructs);
	VSR_CreateGraphicsPipeline(renderer, rendererCreateInfo->subStructs);

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
	VSR_RenderPassDestroy(renderer);

	VSR_SwapchainDestroy(renderer);
	VSR_LogicalDeviceDestroy(renderer);
	VSR_SurfaceDestroy(renderer);
	VSR_InstanceDestroy(renderer);

	/////////////////////////////////
	/// Free renderer's VKStructs ///
	/////////////////////////////////
	SDL_free((void*)renderer->subStructs);
	
	/////////////////////
	/// Free renderer ///
	/////////////////////
	SDL_free((void*)renderer);
}