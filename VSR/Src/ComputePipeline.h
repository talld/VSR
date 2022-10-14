#ifndef VSR_SUITE_COMPUTEPIPELINE_H
#define VSR_SUITE_COMPUTEPIPELINE_H

#include <vulkan/vulkan.h>


typedef struct VSR_ComputePipelineCreateInfo VSR_ComputePipelineCreateInfo;
struct VSR_ComputePipelineCreateInfo
{
	VkPipelineLayoutCreateInfo   computePipelineLayoutCreateInfo;
	VkComputePipelineCreateInfo  computePipelineCreateInfo;
};

#endif //VSR_SUITE_COMPUTEPIPELINE_H
