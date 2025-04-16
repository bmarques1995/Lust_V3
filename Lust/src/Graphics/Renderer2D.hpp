#pragma once

#include "LustDLLMacro.hpp"
#include "OrthographicCamera.hpp"
#include <Eigen/Eigen>
#include "Shader.hpp"
#include "ShaderReflector.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"

namespace Lust
{
	/**
	* @brief %Renderer2D storage
	* @details Contains the renderer2D storage
	* @param m_Shader The shader
	* @param m_ShaderReflector The shader reflector
	* @param m_VertexBuffer The vertex buffer
	* @param m_IndexBuffer The index buffer
	* @param m_UniformBuffer The uniform buffer
	* @param m_Texture The texture
	* @param m_Sampler The sampler
	* @param m_WhiteTexture The white texture
	* @param m_WhiteSampler The white sampler
	* @param m_SSBOInstanceBuffer The raw small buffer
	* @param m_SSBOInstanceBufferSize The raw small buffer size
	*/
	struct Renderer2DStorage
	{
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<ShaderReflector> m_ShaderReflector;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<UniformBuffer> m_UniformBuffer;
		std::shared_ptr<StructuredBuffer> m_StructuredBuffer;
		std::shared_ptr<Texture2D> m_Texture;
		std::shared_ptr<Sampler> m_Sampler;
		std::shared_ptr<Texture2D> m_WhiteTexture;
		std::shared_ptr<Sampler> m_WhiteSampler;
		uint8_t *m_SSBOInstanceBuffer;
		size_t m_SSBOInstanceBufferSize;
		const uint32_t c_MaxInstanceCount = 10000;
		size_t m_InstanceCount;
	};

	/**
	* @brief %Renderer2D class
	* @details %Renderer for 2D elements 
	*/
	class LUST_API Renderer2D
	{
	public:
		/**
		* @brief Renderer2D initializer
		*/
		static void Instantiate();
		/**
		* @brief Renderer2D destructor
		*/
		static void Destroy();

		/**
		* @brief Begins the scene
		* @param camera The camera
		*/
		static void BeginScene(const OrthographicCamera& camera);
		/**
		* @brief Ends the scene
		* @param camera The camera pointer, is only used to access cameras provided by pointer
		*/
		static void BeginScene(const OrthographicCamera* camera);
		/**
		* @brief Ends the scene
		*/
		static void EndScene();
		/**
		* @brief Uploads a texture
		* @param texture The texture
		* @param textureSlot The texture slot
		* @details If you want to use a texture, you have to upload it before call the draw function, the slot 0 is reserved for the white texture,
		* but you can use any other slot until 255, since the renderer uses 256 slots
		*/
		static void UploadTexture2D(const std::shared_ptr<Texture2D>& texture, uint32_t textureSlot = 1);
		/**
		* @brief Uploads a sampler
		* @param sampler The sampler
		* @param samplerSlot The sampler slot
		* @details If you want to use a sampler, you have to upload it before call the draw function, the slot 0 is reserved for the white sampler
		* but you can use any other slot until 15, since the renderer uses 16 sampler slots
		*/
		static void UploadSampler(const std::shared_ptr<Sampler>& sampler, uint32_t samplerSlot = 1);

		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param color The color of the quad
		* @param element_name The element name of the small buffer
		*/
		static void DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, const Eigen::Vector3f& color, std::string_view element_name);
		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param color The color of the quad
		* @param rotation The rotation of the quad
		* @param element_name The element name of the small buffer
		*/
		static void DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, const Eigen::Vector3f& color, float rotation, std::string_view element_name);
		
		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param color The color of the quad
		* @param element_name The element name of the small buffer
		*/
		static void DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector3f& color, std::string_view element_name);
		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param color The color of the quad
		* @param rotation The rotation of the quad
		* @param element_name The element name of the small buffer
		*/
		static void DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector3f& color, float rotation, std::string_view element_name);
		
		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param color The color of the quad, the alpha channel is used to determine if the quad is colored or textured
		* @param controllerInfo A set of 4 uint32_t values, the first one is the texture slot, the second one is the sampler slot, the third one is the level of detail, the fourth one is the mips bias
		* @param element_name The element name of the small buffer
		*/
		static void DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector4f& color, const Eigen::Vector4<uint32_t>& controllerInfo, std::string_view element_name);
		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param color The color of the quad, the alpha channel is used to determine if the quad is colored or textured
		* @param rotation The rotation of the quad
		* @param controllerInfo A set of 4 uint32_t values, the first one is the texture slot, the second one is the sampler slot, the third one is the level of detail, the fourth one is the mips bias
		* @param element_name The element name of the small buffer
		*/
		static void DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector4f& color, float rotation, const Eigen::Vector4<uint32_t>& controllerInfo, std::string_view element_name);

		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param tilingFactor The tiling factor of the quad, is used in the texcoords
		* @param controllerInfo A set of 4 uint32_t values, the first one is the texture slot, the second one is the sampler slot, the third one is the level of detail, the fourth one is the mips bias
		* @param element_name The element name of the small buffer
		*/
		static void DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, float tilingFactor, const Eigen::Vector4<uint32_t>& controllerInfo, std::string_view element_name);
		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param tilingFactor The tiling factor of the quad, is used in the texcoords
		* @param rotation The rotation of the quad
		* @param controllerInfo A set of 4 uint32_t values, the first one is the texture slot, the second one is the sampler slot, the third one is the level of detail, the fourth one is the mips bias
		* @param element_name The element name of the small buffer
		*/
		static void DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, float tilingFactor, float rotation, const Eigen::Vector4<uint32_t>& controllerInfo, std::string_view element_name);
		
		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param tilingFactor The tiling factor of the quad, is used in the texcoords
		* @param controllerInfo A set of 4 uint32_t values, the first one is the texture slot, the second one is the sampler slot, the third one is the level of detail, the fourth one is the mips bias
		* @param element_name The element name of the small buffer
		*/
		static void DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, float tilingFactor, const Eigen::Vector4<uint32_t>& controllerInfo, std::string_view element_name);
		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param tilingFactor The tiling factor of the quad, is used in the texcoords
		* @param rotation The rotation of the quad
		* @param controllerInfo A set of 4 uint32_t values, the first one is the texture slot, the second one is the sampler slot, the third one is the level of detail, the fourth one is the mips bias
		* @param element_name The element name of the small buffer
		*/
		static void DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, float tilingFactor, float rotation, const Eigen::Vector4<uint32_t>& controllerInfo, std::string_view element_name);
	private:
		/**
		* @brief Renders a quad
		* @param squareSmallBufferMatrix The matrix of the small buffer
		* @param color The color of the quad
		* @param element_names The element names of the small buffer
		* @param controllerInfo A set of 4 uint32_t values, the first one is the texture slot, the second one is the sampler slot, the third one is the level of detail, the fourth one is the mips bias
		*/
		static void RenderPush(const Eigen::Matrix4f& squareSmallBufferMatrix, const Eigen::Vector4f& color, std::string_view element_names, const Eigen::Vector4<uint32_t>& controllerInfo);

		/**
		* @brief Dispatches the draws
		*/
		static void DispatchDraws();

		/**
		* @brief Returns the complete MVP
		* @param model the model matrix
		* @param view the view matrix
		* @param projection the projection matrix
		* @param mipLevel the mip level matrix, condensing some mips levels
		*/
		struct CompleteMVP
		{
			Eigen::Matrix4f model;
			Eigen::Matrix4f view;
			Eigen::Matrix4f projection;
			Eigen::Matrix4f mipLevel;
		};

		struct SmallMVP
		{
			Eigen::Matrix4f model;
		};

		static std::shared_ptr<Renderer2DStorage> s_Renderer2DStorage;
		static std::unique_ptr<SceneData> s_SceneData;
	};
}