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
	pipeline->renderTarget = createInfo->renderTarget;

	uint32_t renderWidth, renderHeight;
	VSR_GraphicsPipelineGetRenderSize(
		renderer,
		pipeline,
		&renderWidth,
		&renderHeight
	);

	SDL_Surface* depthSur = SDL_CreateRGBSurfaceWithFormat(
		0,
		renderWidth,
		renderHeight,
		24,
		SDL_PIXELFORMAT_BGR888
	);
	depthSur->pixels = NULL;

	pipeline->depthImage = VSR_ImageCreate(
		renderer,
		depthSur,
		VK_FORMAT_D32_SFLOAT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
	);

	pipeline->depthView = VSR_ImageViewCreate(
		renderer,
		pipeline->depthImage->image,
		pipeline->depthImage->format,
		VK_IMAGE_ASPECT_DEPTH_BIT
	);
	SDL_FreeSurface(depthSur);

	if(pipeline->renderTarget)
	{ // custom target

		pipeline->renderPass = VSR_RenderPassCreate(
			renderer,
			pipeline,
			pipeline->renderTarget->view,
			pipeline->depthView
		);

		pipeline->framebuffers = SDL_malloc(sizeof(VSR_Framebuffer*));

		pipeline->framebuffers[0] = VSR_FramebufferCreate(
			renderer,
			pipeline,
			pipeline->renderTarget->view
		);
	}
	else
	{ // use swapchain images

		pipeline->renderPass = VSR_RenderPassCreate(
			renderer,
			pipeline,
			renderer->swapchain.pImageViews[0],
			pipeline->depthView
		);


		pipeline->framebuffers = SDL_malloc(
			sizeof(VSR_Framebuffer*) * renderer->swapchainImageCount
		);

		for(size_t i = 0; i < renderer->swapchainImageCount; i++)
		{
			pipeline->framebuffers[i] = VSR_FramebufferCreate(
				renderer,
				pipeline,
				renderer->swapchain.pImageViews[i]
			);
		}
	}

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
	Renderer_WaitOnGenerationalFence(
		renderer,
		renderer->swapchainImageCount,
		&renderer->imageFinished[renderer->currentFrame],
		renderer->generationAcquired[renderer->currentFrame]
	);

	VSR_ImageViewDestroy(renderer, pipeline->depthView);
	VSR_ImageDestroy(renderer, pipeline->depthImage);

	if(pipeline->renderTarget)
	{ // custom target

		VSR_FramebufferDestroy(renderer, pipeline->framebuffers[0]);
	}
	else
	{ // was using spwapchain images
		for(size_t i = 0; i < renderer->swapchainImageCount; i++)
		{

				VSR_FramebufferDestroy(renderer, pipeline->framebuffers[i]);
		}
	}
	SDL_free(pipeline->framebuffers);

	VSR_RenderPassDestroy(renderer, pipeline->renderPass);

	GraphicsPipeline_GraphicPipelineDestroy(renderer, pipeline);

	SDL_free(pipeline);
}





//==============================================================================
// VSR_GraphicsPipelineGetRenderSize
//------------------------------------------------------------------------------
void
VSR_GraphicsPipelineGetRenderSize(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	uint32_t* w,
	uint32_t* h)
{
	if(pipeline->renderTarget)
	{
		*w = pipeline->renderTarget->image->src.w;
		*h = pipeline->renderTarget->image->src.h;
	}
	else
	{
		*w = renderer->surface.surfaceWidth;
		*h = renderer->surface.surfaceHeight;
	}
}





//==============================================================================
// VSR_GraphicsPipelineSetRenderTarget
//------------------------------------------------------------------------------
int
VSR_GraphicsPipelineSetRenderTarget(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo * pipelineCreateInfo,
	VSR_Sampler* sampler)
{
	pipelineCreateInfo->renderTarget = sampler;

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
