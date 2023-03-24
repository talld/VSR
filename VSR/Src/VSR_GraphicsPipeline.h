#ifndef VSR_SUITE_VSR_GRAPHICSPIPELINE_H
#define VSR_SUITE_VSR_GRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>

#include "VSR_Shader.h"
#include "VSR_Image.h"
#include "Renderer_DescriptorPool.h"
#include "GraphicsPipeline_RenderPass.h"
#include "GraphicsPipeline_GraphicsPipeline.h"
#include "GraphicsPipeline_Framebuffer.h"
#include "Renderer_CommandPool.h"


typedef struct VSR_GraphicsPipelineCreateInfo VSR_GraphicsPipelineCreateInfo;
struct VSR_GraphicsPipelineCreateInfo
{
	VSR_Shader*                             vertexShader;
	VSR_Shader*                             fragmentShader;

	GraphicsPipeline_RenderPassCreateInfo       renderPassCreateInfo;
	GraphicsPipeline_FramebufferCreateInfo      framebufferCreateInfo;
	GraphicsPipeline_GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
};

typedef struct VSR_GraphicsPipeline VSR_GraphicsPipeline;
struct VSR_GraphicsPipeline
{
	VSR_Image*                         depthImage;
	VSR_ImageView*                     depthView;

	GraphicsPipeline_RenderPass        renderPass;
	GraphicsPipeline_Framebuffer       framebuffer;
	GraphicsPipeline_GraphicsPipeline  graphicPipeline;
};

#endif //VSR_SUITE_VSR_GRAPHICSPIPELINE_H
