#pragma once

#include "Texture.hpp"
#include "VKContext.hpp"

namespace Lust
{
	class LUST_API VKTexture2D : public Texture2D
	{
	public:
		VKTexture2D(const VKContext* context, const TextureBuffer& specification);
		~VKTexture2D();

		const TextureBuffer& GetTextureDescription() const override;

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		bool IsLoaded() const override;
	
		VkImage GetResource() const;
		VmaAllocation GetAllocation() const;
		VkImageView GetView() const;

	private:
		void CreateResource();
		void CopyBuffer();

		//uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		static VkImageType GetNativeTensor(TextureTensor tensor);
		static VkImageViewType GetNativeTensorView(TextureTensor tensor);

		const VKContext* m_Context;
		bool m_Loaded;

		TextureBuffer m_Specification;

		VkImage m_Resource;
		VmaAllocation m_Allocation;
		VkImageView m_ResourceView;
	};
}