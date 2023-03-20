#include "VSR_Sampler.h"

#include "VSR_Renderer.h"
#include "fallbackTexture.h"

void VSR_SamplerWriteToDescriptor(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	size_t index,
	VSR_Sampler* sampler
	)
{
	VkDescriptorImageInfo imageInfo;
	imageInfo.sampler = sampler->sampler;
	imageInfo.imageView = sampler->view.imageView;
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet imageWrite = (VkWriteDescriptorSet){0};
	imageWrite.pNext = NULL;

	imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	imageWrite.dstSet = renderer->subStructs->descriptorPool.globalSet;
	imageWrite.dstBinding = 0;
	imageWrite.dstArrayElement = index;
	imageWrite.descriptorCount = 1;
	imageWrite.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(renderer->subStructs->logicalDevice.device,
						   1, &imageWrite,
						   0, NULL);
}

VSR_Sampler*
VSR_SamplerCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	size_t index,
	SDL_Surface* sur)
{
	VSR_Sampler* sampler = SDL_malloc(sizeof(VSR_Sampler));

	VSR_Image* img = VSR_ImageCreate(
		renderer,
		pipeline,
		sur,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
		| VK_IMAGE_USAGE_SAMPLED_BIT
		| VK_BUFFER_USAGE_TRANSFER_DST_BIT
	);

	VSR_ImageTransition(
		renderer,
		pipeline,
		img,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);

	VSR_ImageView imgView = VSR_ImageViewCreate(
		renderer,
		img->image,
		img->format,
		VK_IMAGE_ASPECT_COLOR_BIT
	);

	///////////////////////////////////
	/// populate and return sampler ///
	///////////////////////////////////
	sampler->index = index;
	sampler->image = img;
	sampler->view = imgView;
	sampler->sampler = VSR_GetTextureSampler(renderer);

	VSR_SamplerWriteToDescriptor(renderer,pipeline, index, sampler);

	return sampler;
}

VkSampler
VSR_GetTextureSampler(
	VSR_Renderer* renderer)
{
	static VkSampler sTextureSampler;

	if(!sTextureSampler)
	{
		VkSamplerCreateInfo textureSamplerInfo = (VkSamplerCreateInfo){0};
		textureSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		textureSamplerInfo.pNext = NULL;
		textureSamplerInfo.flags = 0L;
		textureSamplerInfo.magFilter = VK_FILTER_LINEAR;
		textureSamplerInfo.minFilter = VK_FILTER_LINEAR;
		textureSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		textureSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		textureSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		textureSamplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

		textureSamplerInfo.unnormalizedCoordinates = VK_FALSE;
		textureSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		textureSamplerInfo.mipLodBias = 0.f;
		textureSamplerInfo.minLod = 0.f;
		textureSamplerInfo.maxLod = 0.f;

		textureSamplerInfo.anisotropyEnable = VK_FALSE;
		textureSamplerInfo.maxAnisotropy = 0.f;

		vkCreateSampler(renderer->subStructs->logicalDevice.device,
						&textureSamplerInfo,
						VSR_GetAllocator(),
						&sTextureSampler);
	}

	return sTextureSampler;
}

void
VSR_PopulateDefaultSamplers(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline)
{
	SDL_Surface* sur = SDL_CreateRGBSurfaceWithFormat(
		0,
		kFallBackTextureWidth,
		kFallBackTextureHeight,
		kFallBackTextureDepth,
		kFallBackTextureFormat);
	sur->pixels = kFallBackTexturePixels;

	 VSR_Sampler* sampler = VSR_SamplerCreate(renderer, pipeline, 0, sur);

	for(size_t i = 0; i < renderer->subStructs->texturePoolSize; i++)
	{
		VSR_SamplerWriteToDescriptor(renderer,pipeline, i, sampler);
	}
}