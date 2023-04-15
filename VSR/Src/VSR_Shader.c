#include "VSR_Shader.h"

#include "VSR_error.h"
#include "VSR_Renderer.h"





//==============================================================================
// VSR_ShaderCreate
//------------------------------------------------------------------------------
VSR_Shader*
VSR_ShaderCreate(
	VSR_Renderer* renderer,
	size_t byteCount,
	const uint8_t* bytes)
{
	if(bytes == NULL || byteCount == 0)
	{
		VSR_SetErr("Shader creation failed: invalid bytes");
		goto FAIL;
	}


	VSR_Shader* shader = SDL_malloc(sizeof(VSR_Shader));

	shader->createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader->createInfo.pNext = NULL;
	shader->createInfo.flags = 0L;

	shader->createInfo.codeSize = byteCount;
	shader->createInfo.pCode = (uint32_t*) bytes;

	vkCreateShaderModule(
		renderer->logicalDevice.device,
		&shader->createInfo,
		VSR_GetAllocator(),
		&shader->module
		);

	SUCCESS:
	return shader;

	FAIL:
	return NULL;
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
		renderer->logicalDevice.device,
		shader->module,
		VSR_GetAllocator()
		);

	SDL_free(shader);
}
