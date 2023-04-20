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
	GraphicsPipeline_GraphicsPipelinePopulateCreateInfo(renderer, createInfo);

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

	GraphicsPipeline_GraphicsPipelineCreate(renderer, pipeline, createInfo);

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
	// wait for the current frame to finish
	vkWaitForFences(
		renderer->logicalDevice.device,
		1,
		&renderer->imageFinished[renderer->currentFrame],
		VK_TRUE,
		-1
	);

	GraphicsPipeline_GraphicPipelineDestroy(renderer, pipeline);

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
