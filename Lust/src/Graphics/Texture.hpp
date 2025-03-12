#pragma once

#include <cstdint>
#include <string>
#include "GraphicsContext.hpp"
#include "TextureLayout.hpp"
#include "CopyPipeline.hpp"

namespace Lust
{
	class LUST_API Texture
	{
	public:
		virtual ~Texture() = default;

		virtual const TextureBuffer& GetTextureDescription() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual bool IsLoaded() const = 0;

		const std::string& GetName() const;

		void SetName(const std::string& name);

	protected:
		std::string m_Name;
	};

	class LUST_API TextureLibrary
	{
	public:
		static void InitCopyPipeline(const GraphicsContext* context);
		static void DestroyCopyPipeline();

		static std::shared_ptr<CopyPipeline>* GetCopyPipeline();
	protected:
		static std::shared_ptr<CopyPipeline> s_CopyPipeline;
		static bool s_IsInitialized;
	};

	class LUST_API Texture2D : public Texture
	{
	public:
		Texture2D(const TextureBuffer& specs);
		virtual ~Texture2D() = default;
		// For Vulkan, I'm using the location mapping slot and spaceSet(std140(slot, spaceSet)) to locate the texture
		// For D3D12, I'm using the heapSlot(RootSignature descriptor range index) and textureIndex(a descriptor table can handle multiple textures)
		// In the moment that I add shaders preprocessing, I will set these parameters to the shader result
		static Texture2D* Instantiate(const GraphicsContext* context, const std::string& path);
		static Texture2D* Instantiate(const GraphicsContext* context, uint32_t width, uint32_t height, std::string name);
	};

	class LUST_API Texture2DLibrary
	{
	public:
		Texture2DLibrary(const GraphicsContext* context);
		~Texture2DLibrary();

		void Add(const std::string& name, const std::shared_ptr<Texture2D>& texture);
		void Add(const std::shared_ptr<Texture2D>& texture);
		std::shared_ptr<Texture2D> Load(const std::string& filepath);
		std::shared_ptr<Texture2D> Load(const std::string& name, const std::string& filepath);

		std::shared_ptr<Texture2D> Get(const std::string& name);

		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_Textures;
		const GraphicsContext* m_Context;
	};
}
