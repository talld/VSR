#include "VSR_GraphicsPipeline.h"

#include <vulkan/vulkan.h>
#include "VSR_Renderer.h"





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


	GraphicsPipeline_RenderPassPopulateCreateInfo(renderer, createInfo, createInfo->subStructs);
	GraphicsPipeline_GraphicsPipelinePopulateCreateInfo(renderer, createInfo, createInfo->subStructs);
	GraphicsPipeline_FramebufferPopulateCreateInfo(renderer, createInfo, createInfo->subStructs);
	GraphicsPipeline_CommandPoolPopulateCreateInfo(renderer, createInfo, createInfo->subStructs);

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

	GraphicsPipeline_RenderPassCreate(renderer, pipeline, createInfo->subStructs);
	GraphicsPipeline_GraphicsPipelineCreate(renderer, pipeline, createInfo->subStructs);
	GraphicsPipeline_FramebufferCreate(renderer, pipeline, createInfo->subStructs);
	GraphicsPipeline_CommandPoolCreate(renderer, pipeline, createInfo->subStructs);

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
	vkDeviceWaitIdle(renderer->subStructs->logicalDevice.device);

	GraphicsPipeline_CommandPoolDestroy(renderer, pipeline);
	GraphicsPipeline_FramebufferDestroy(renderer, pipeline);
	GraphicsPipeline_GraphicPipelineDestroy(renderer, pipeline);
	GraphicsPipeline_RenderPassDestroy(renderer, pipeline);

	SDL_free(pipeline->subStructs);
	SDL_free(pipeline);
}