#ifndef VSR_SUITE_VSR_GRAPHICSPIPELINE_H
#define VSR_SUITE_VSR_GRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>

#include "VSR_Shader.h"
#include "VSR_Image.h"
#include "Renderer_DescriptorPool.h"
#include "GraphicsPipeline_RenderPass.h"
#include "GraphicsPipeline_GraphicsPipeline.h"
#include "VSR_Framebuffer.h"
#include "Renderer_CommandPool.h"


typedef struct VSR_GraphicsPipelineCreateInfo VSR_GraphicsPipelineCreateInfo;
struct VSR_GraphicsPipelineCreateInfo
{
	VSR_Shader*                             vertexShader;
	VSR_Shader*                             fragmentShader;

	GraphicsPipeline_GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
};

typedef struct VSR_GraphicsPipeline VSR_GraphicsPipeline;
struct VSR_GraphicsPipeline
{
	GraphicsPipeline_GraphicsPipeline  graphicPipeline;

	VSR_Sampler* renderTarget;
	VkFence    renderTargetFinished;

	VSR_PushConstants pushConstants;
};

int
VSR_PipelineSetRenderTarget(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VSR_Sampler* sampler);


#endif //VSR_SUITE_VSR_GRAPHICSPIPELINE_H
