#include "VSR_GraphicsPipeline.h"

#include <vulkan/vulkan.h>
#include "VSR_Renderer.h"

#include "VSR_Image.h"
#include "VSR_Sampler.h"
#include "Renderer_Memory.h"



//==============================================================================
// VSR_GraphicsPipelineGenerateCreateInfo
//------------------------------------------------------------------------------
VSR_GraphicsPipelineCreateInfo*
VSR_GraphicsPipelineGenerateCreateInfo(
	VSR_Renderer* renderer
	)
{
	VSR_GraphicsPipelineCreateInfo* createInfo = SDL_calloc(1, sizeof(VSR_GraphicsPipelineCreateInfo));
	createInfo->subStructs = SDL_calloc(1, sizeof(GraphicsPipeline_CreateInfoSubStructs));

	/////////////////////
	/// init defaults ///
	/////////////////////

	renderer->subStructs->texturePoolSize = 256;

	//////////////////////
	/// populate infos ///
	//////////////////////

	GraphicsPipeline_DescriptorPoolPopulateCreateInfo(renderer, createInfo);
	GraphicsPipeline_RenderPassPopulateCreateInfo(renderer, createInfo);
	GraphicsPipeline_GraphicsPipelinePopulateCreateInfo(renderer, createInfo);
	GraphicsPipeline_FramebufferPopulateCreateInfo(renderer, createInfo);
	GraphicsPipeline_CommandPoolPopulateCreateInfo(renderer, createInfo);

	return createInfo;
}





//==============================================================================
// VSR_GraphicsPipelineCreateInfoFree
//------------------------------------------------------------------------------
void VSR_GraphicsPipelineCreateInfoFree(
	VSR_GraphicsPipelineCreateInfo* createInfo)
{
	SDL_free(createInfo->subStructs);
	SDL_free(createInfo);
}





//==============================================================================
// VSR_GraphicsPipelineCreate
//------------------------------------------------------------------------------
VSR_GraphicsPipeline*
VSR_GraphicsPipelineCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo
)
{
	VSR_GraphicsPipeline* pipeline = SDL_calloc(1, sizeof(VSR_GraphicsPipeline));
	pipeline->subStructs = SDL_calloc(1, sizeof(GraphicsPipeline_SubStructs));

	// TODO: refactor this into a function that will gauge depthsize
	//  and create a surface with those requirements ( keep alignment! )
	SDL_Surface* depthSur = SDL_CreateRGBSurfaceWithFormat(
		0,
		renderer->subStructs->surface.surfaceWidth,
		renderer->subStructs->surface.surfaceHeight,
		24,
		SDL_PIXELFORMAT_BGR888
	);
	depthSur->pixels = NULL;

	pipeline->subStructs->depthImage = VSR_ImageCreate(
		renderer,
		pipeline,
		depthSur,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
	);

	pipeline->subStructs->depthView = VSR_ImageViewCreate(
		renderer,
		pipeline->subStructs->depthImage->image,
		pipeline->subStructs->depthImage->format,
		VK_IMAGE_ASPECT_DEPTH_BIT
	);

	GraphicsPipeline_DescriptorPoolCreate(renderer, pipeline, createInfo);
	GraphicsPipeline_RenderPassCreate(renderer, pipeline, createInfo);
	GraphicsPipeline_GraphicsPipelineCreate(renderer, pipeline, createInfo);
	GraphicsPipeline_FramebufferCreate(renderer, pipeline, createInfo);
	GraphicsPipeline_CommandPoolCreate(renderer, pipeline, createInfo);

	renderer->subStructs->pipeline = pipeline;
	VSR_PopulateDefaultSamplers(renderer, pipeline);

	return pipeline;
}





//==============================================================================
// VSR_GraphicsPipelineFree
//------------------------------------------------------------------------------
void
VSR_GraphicsPipelineFree(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline)
{
	VSR_ImageViewDestroy(
		renderer,
		&pipeline->subStructs->depthView
		);

	vkDeviceWaitIdle(renderer->subStructs->logicalDevice.device);

	GraphicsPipeline_CommandPoolDestroy(renderer, pipeline);
	GraphicsPipeline_FramebufferDestroy(renderer, pipeline);
	GraphicsPipeline_GraphicPipelineDestroy(renderer, pipeline);
	GraphicsPipeline_RenderPassDestroy(renderer, pipeline);
	GraphicsPipeline_DescriptorPoolDestroy(renderer, pipeline);

	SDL_free(pipeline->subStructs);
	SDL_free(pipeline);
}

void
VSR_GraphicsPipelineSetShader(
	VSR_GraphicsPipelineCreateInfo* pipeline,
	VSR_ShaderStage stage,
	VSR_Shader* shader)
{
	if(stage == SHADER_STAGE_VERTEX)
	{
		pipeline->vertexShader = shader;
	}

	if(stage == SHADER_STAGE_FRAGMENT)
	{
		pipeline->fragmentShader = shader;
	}
}