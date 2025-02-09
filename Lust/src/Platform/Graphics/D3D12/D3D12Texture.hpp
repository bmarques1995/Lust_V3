#pragma once

#ifdef LUST_USES_WINDOWS

#include "Texture.hpp"
#include "D3D12Context.hpp"

namespace Lust
{
	class LUST_API D3D12Texture2D : public Texture2D
	{
	public:
		D3D12Texture2D(const D3D12Context* context, const TextureElement& specification);
		~D3D12Texture2D();

		const TextureElement& GetTextureDescription() const override;

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		bool IsLoaded() const override;

		ID3D12Resource2* GetResource() const;

	private:
		void UpdateTextureInfo(const D3D12_RESOURCE_DESC1& desc);
		void CreateResource();
		void CopyBuffer();

		static D3D12_RESOURCE_DIMENSION GetNativeTensor(TextureTensor tensor);

		const D3D12Context* m_Context;
		bool m_Loaded;
		TextureElement m_Specification;

		ComPointer<ID3D12Resource2> m_Texture;
	};
}

#endif