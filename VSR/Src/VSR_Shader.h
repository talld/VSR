#ifndef VSR_SUITE_VSR_SHADER_H
#define VSR_SUITE_VSR_SHADER_H

#include <vulkan/vulkan.h>

typedef enum VSR_ShaderStage VSR_ShaderStage;
enum VSR_ShaderStage
{
	SHADER_STAGE_FRAGMENT = 0,
	SHADER_STAGE_VERTEX = 1,
};

typedef struct VSR_Shader VSR_Shader;
struct VSR_Shader
{
	VkShaderModuleCreateInfo createInfo;
	VkShaderModule           module;
};

VSR_Shader
VSR_ShaderCreate(
	VSR_Renderer* renderer,
	size_t byteCount,
	const uint8_t* bytes);

void
VSR_ShaderDestroy(
	VSR_Renderer* renderer,
	VSR_Shader* shader);


#endif //VSR_SUITE_VSR_SHADER_H
