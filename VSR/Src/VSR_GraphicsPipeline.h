#ifndef VSR_SUITE_VSR_GRAPHICSPIPELINE_H
#define VSR_SUITE_VSR_GRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>

#include "VSR_Shader.h"
#include "GraphicsPipeline_RenderPass.h"
#include "GraphicsPipeline_GraphicsPipeline.h"
#include "GraphicsPipeline_Framebuffer.h"
#include "GraphicsPipeline_CommandPool.h"

typedef struct GraphicsPipeline_CreateInfoSubStructs GraphicsPipeline_CreateInfoSubStructs;
struct GraphicsPipeline_CreateInfoSubStructs
{
	GraphicsPipeline_RenderPassCreateInfo      renderPassCreateInfo;
	GraphicsPipeline_GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
	GraphicsPipeline_FramebufferCreateInfo     framebufferCreateInfo;
	GraphicsPipeline_CommandPoolCreateInfo     commandPoolCreateInfo;
};

typedef struct VSR_GraphicsPipelineCreateInfo VSR_GraphicsPipelineCreateInfo;
struct VSR_GraphicsPipelineCreateInfo
{
	VSR_Shader*                             vertexShader;
	VSR_Shader*                             fragmentShader;
	GraphicsPipeline_CreateInfoSubStructs*  subStructs;
};

typedef struct GraphicsPipeline_SubStructs GraphicsPipeline_SubStructs;
struct GraphicsPipeline_SubStructs
{
	GrapicsPipeline_RenderPass         renderPass;
	GraphicsPipeline_GraphicsPipeline  graphicPipeline;
	GraphicsPipeline_Framebuffer       framebuffer;
	GraphicsPipeline_CommandPool       commandPool;
};

typedef struct VSR_GraphicsPipeline VSR_GraphicsPipeline;
struct VSR_GraphicsPipeline
{
	GraphicsPipeline_SubStructs* subStructs;
};

#endif //VSR_SUITE_VSR_GRAPHICSPIPELINE_H
