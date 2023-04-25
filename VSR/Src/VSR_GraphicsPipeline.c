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
	pipeline->renderTarget = NULL;

	GraphicsPipeline_GraphicsPipelineCreate(renderer, pipeline, createInfo);

	return pipeline;
}




//==============================================================================
// VSR_PipelineSetRenderTarget
//------------------------------------------------------------------------------
int
VSR_GraphicsPipelineSetRenderTarget(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VSR_Sampler* sampler)
{
	if(sampler)
	{
		VSR_ImageTransition(
			renderer,
			sampler->image,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		);

		// take target out of texture index
		VSR_SamplerWriteToDescriptor(
			renderer,
			sampler->textureIndex,
			renderer->defaultSampler
		);
	}

	if(pipeline->renderTarget && pipeline->renderTarget != sampler)
	{
		VSR_SamplerWriteToDescriptor(
			renderer,
			pipeline->renderTarget->textureIndex,
			pipeline->renderTarget
		);

		VSR_ImageTransition(
			renderer,
			pipeline->renderTarget->image,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		);

		// put sampler back in the texture array
		VSR_SamplerWriteToDescriptor(
			renderer,
			pipeline->renderTarget->textureIndex,
			pipeline->renderTarget
		);
	}

	pipeline->renderTarget = sampler;

	SUCCESS:
	return SDL_FALSE;
	FAIL:
	return SDL_TRUE;
}

//==============================================================================
// VSR_RendererSetVertexConstants
//------------------------------------------------------------------------------
void
VSR_GraphicsPipelineSetPushConstants(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VSR_PushConstants const* pushConstants)
{
	pipeline->pushConstants = *pushConstants;
}


//==============================================================================
// VSR_GraphicsPipelineFree
//------------------------------------------------------------------------------
void
VSR_GraphicsPipelineFree(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline)
{
	Renderer_WaitOnGenerationalFence(
		renderer,
		renderer->swapchainImageCount,
		&renderer->imageFinished[renderer->currentFrame],
		renderer->generationAcquired[renderer->currentFrame]
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
