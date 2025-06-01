#pragma once

#include <cstdint>
#include <string>
#include "GraphicsContext.hpp"
#include "TextureLayout.hpp"
#include "CopyPipeline.hpp"
#include "Image.hpp"

// For Vulkan, I'm using the location mapping slot and spaceSet(std140(slot, spaceSet)) to locate the texture
// For D3D12, I'm using the heapSlot(RootSignature descriptor range index) and textureIndex(a descriptor table can handle multiple textures)
// In the moment that I add shaders preprocessing, I will set these parameters to the shader result

namespace Lust
{
	/**
	* @brief Interface for a texture
	* @details Handles texture GPU buffer
	*/
	class LUST_API Texture
	{
	public:
		/**
		* @brief Texture destructor
		*/
		virtual ~Texture() = default;

		/**
		* @brief Get texture description
		*/
		virtual const TextureBuffer& GetTextureDescription() const = 0;

		/**
		* @brief Get texture width
		*/
		virtual uint32_t GetWidth() const = 0;
		/**
		* @brief Get texture height
		*/
		virtual uint32_t GetHeight() const = 0;

		/**
		* @brief Is texture loaded
		*/
		virtual bool IsLoaded() const = 0;

		/**
		* @brief Get texture name
		*/
		const std::string& GetName() const;

		/**
		* @brief Set texture name
		*/
		void SetName(const std::string& name);

	protected:
		std::string m_Name;
	};

	/**
	* @brief Interface for a texture library, base class that handles common texture library components
	*/
	class LUST_API TextureLibrary
	{
	public:
		/**
		* @brief Initialize copy pipeline, that is used to copy textures
		*/
		static void InitCopyPipeline(const GraphicsContext* context);
		/**
		* @brief Destroy copy pipeline
		*/
		static void DestroyCopyPipeline();

		/**
		* @brief Get copy pipeline
		*/
		static std::shared_ptr<CopyPipeline>* GetCopyPipeline();
	protected:
		static std::shared_ptr<CopyPipeline> s_CopyPipeline;
		static bool s_IsInitialized;
	};

	/**
	* @brief Interface for a 2D texture
	*/
	class LUST_API Texture2D : public Texture
	{
	public:
		/**
		* @brief Texture2D constructor
		*/
		Texture2D(const TextureBuffer& specs);
		/**
		* @brief Texture2D destructor
		*/
		virtual ~Texture2D() = default;
		/**
		* @brief Instantiate texture
		* @param context The graphics context
		* @param path The texture path
		* @details Factory method
		*/
		static Texture2D* Instantiate(const GraphicsContext* context, const std::string& path);
		/**
		* @brief Instantiate texture
		* @param context The graphics context
		* @param width The texture width
		* @param height The texture height
		* @param name The texture name
		*/
		static Texture2D* Instantiate(const GraphicsContext* context, uint32_t width, uint32_t height, std::string name);
		/**
		* @brief Instantiate texture
		* @param context The graphics context
		* @param image The texture buffer
		* @details Factory method
		*/
		static Texture2D* Instantiate(const GraphicsContext* context, const std::shared_ptr<Image> image, std::string name);
	};

	/**
	* @brief Interface for a 2D texture library
	* @details Handles 2D textures
	*/
	class LUST_API Texture2DLibrary
	{
	public:
		/**
		* @brief Texture2DLibrary constructor
		* @param context The graphics context
		*/
		Texture2DLibrary(const GraphicsContext* context);
		/**
		* @brief Texture2DLibrary destructor
		*/
		~Texture2DLibrary();

		/**
		* @brief Add texture
		* @param name The texture name
		* @param texture The texture
		*/
		void Add(const std::string& name, const std::shared_ptr<Texture2D>& texture);
		/**
		* @brief Add texture
		* @param texture The texture
		*/
		void Add(const std::shared_ptr<Texture2D>& texture);
		/**
		* @brief Load texture
		* @param filepath The texture filepath
		*/
		std::shared_ptr<Texture2D> Load(const std::string& filepath);
		/**
		* @brief Load texture
		* @param image The texture buffer
		*/
		std::shared_ptr<Texture2D> Load(const std::shared_ptr<Image>& image, const std::string& name);
		/**
		* @brief Load texture
		* @param name The texture name
		* @param filepath The texture filepath
		*/
		std::shared_ptr<Texture2D> Load(const std::string& name, const std::string& filepath);

		/**
		* @brief Get texture
		*/
		std::shared_ptr<Texture2D> Get(const std::string& name);

		/**
		* @brief Check if texture exists
		*/
		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_Textures;
		const GraphicsContext* m_Context;
	};
}
