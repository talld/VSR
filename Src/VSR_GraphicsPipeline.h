#ifndef VSR_SUITE_VSR_GRAPHICSPIPELINE_H
#define VSR_SUITE_VSR_GRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>

#include "VSR_Shader.h"
#include "VSR_Image.h"
#include "Renderer_DescriptorPool.h"
#include "VSR_RenderPass.h"
#include "GraphicsPipeline_GraphicsPipeline.h"
#include "VSR_Framebuffer.h"
#include "Renderer_CommandPool.h"


typedef struct VSR_GraphicsPipelineCreateInfo VSR_GraphicsPipelineCreateInfo;
struct VSR_GraphicsPipelineCreateInfo
{
	VSR_Shader*  vertexShader;
	VSR_Shader*  fragmentShader;
	VSR_Sampler* renderTarget;

	GraphicsPipeline_GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
};

typedef struct VSR_GraphicsPipeline VSR_GraphicsPipeline;
struct VSR_GraphicsPipeline
{
	GraphicsPipeline_GraphicsPipeline  graphicPipeline;
	VSR_RenderPass* renderPass;

	VSR_Image*     depthImage;
	VSR_ImageView* depthView;

	VSR_Sampler* renderTarget;
	VkFence    renderTargetFinished;

	VSR_Framebuffer** framebuffers;

	VSR_PushConstants pushConstants;
};

void
VSR_GraphicsPipelineGetRenderSize(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	uint32_t* w,
	uint32_t* h);

#endif //VSR_SUITE_VSR_GRAPHICSPIPELINE_H
