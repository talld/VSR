#ifndef VSR_SUITE_VSR_SHADER_H
#define VSR_SUITE_VSR_SHADER_H

#include <VSR.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

typedef struct VSR_Shader VSR_Shader;
struct VSR_Shader
{
	VkShaderModuleCreateInfo createInfo;
	VkShaderModule           module;
};

VSR_Shader*
VSR_ShaderCreate(
	VSR_Renderer* renderer,
	size_t byteCount,
	const uint8_t* bytes);

void
VSR_ShaderDestroy(
	VSR_Renderer* renderer,
	VSR_Shader* shader);

#endif //VSR_SUITE_VSR_SHADER_H