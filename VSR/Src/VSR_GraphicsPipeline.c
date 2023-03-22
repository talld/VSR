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

	/////////////////////
	/// init defaults ///
	/////////////////////

	//////////////////////
	/// populate infos ///
	//////////////////////


	GraphicsPipeline_RenderPassPopulateCreateInfo(renderer, createInfo);
	GraphicsPipeline_GraphicsPipelinePopulateCreateInfo(renderer, createInfo);
	GraphicsPipeline_FramebufferPopulateCreateInfo(renderer, createInfo);

	return createInfo;
}





//==============================================================================
// VSR_GraphicsPipelineCreateInfoFree
//------------------------------------------------------------------------------
void VSR_GraphicsPipelineCreateInfoFree(
	VSR_GraphicsPipelineCreateInfo* createInfo)
{
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

	// TODO: refactor this into a function that will gauge depthsize
	//  and create a surface with those requirements ( keep alignment! )
	SDL_Surface* depthSur = SDL_CreateRGBSurfaceWithFormat(
		0,
		renderer->surface.surfaceWidth,
		renderer->surface.surfaceHeight,
		24,
		SDL_PIXELFORMAT_BGR888
	);
	depthSur->pixels = NULL;

	pipeline->depthImage = VSR_ImageCreate(
		renderer,
		pipeline,
		depthSur,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
	);

	pipeline->depthView = VSR_ImageViewCreate(
		renderer,
		pipeline->depthImage->image,
		pipeline->depthImage->format,
		VK_IMAGE_ASPECT_DEPTH_BIT
	);

	GraphicsPipeline_RenderPassCreate(renderer, pipeline, createInfo);
	GraphicsPipeline_FramebufferCreate(renderer, pipeline, createInfo);
	GraphicsPipeline_GraphicsPipelineCreate(renderer, pipeline, createInfo);

	renderer->pipeline = pipeline;
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
		&pipeline->depthView
		);

	vkDeviceWaitIdle(renderer->logicalDevice.device);

	GraphicsPipeline_FramebufferDestroy(renderer, pipeline);
	GraphicsPipeline_GraphicPipelineDestroy(renderer, pipeline);
	GraphicsPipeline_RenderPassDestroy(renderer, pipeline);

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