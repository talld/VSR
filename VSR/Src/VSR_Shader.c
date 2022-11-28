#include "VSR_Shader.h"

#include "Renderer.h"





//==============================================================================
// VSR_ShaderCreate
//------------------------------------------------------------------------------
VSR_Shader
VSR_ShaderCreate(
	VSR_Renderer* renderer,
	size_t byteCount,
	const uint8_t* bytes)
{
	VSR_Shader shader;

	shader.createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader.createInfo.pNext = NULL;
	shader.createInfo.flags = 0L;

	shader.createInfo.codeSize = byteCount;
	shader.createInfo.pCode = (uint32_t*) bytes;

	vkCreateShaderModule(
		renderer->subStructs->logicalDevice.device,
		&shader.createInfo,
		VSR_GetAllocator(),
		&shader.module
		);

	return shader;
}





//==============================================================================
// VSR_ShaderDestroy
//------------------------------------------------------------------------------
void
VSR_ShaderDestroy(
	VSR_Renderer* renderer,
	VSR_Shader* shader)
{
	vkDestroyShaderModule(
		renderer->subStructs->logicalDevice.device,
		shader->module,
		VSR_GetAllocator()
		);
}
