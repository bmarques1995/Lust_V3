#pragma once

#include "LustDLLMacro.hpp"
#include "OrthographicCamera.hpp"
#include "Camera.hpp"
#include <Eigen/Eigen>
#include "Shader.hpp"
#include "ShaderReflector.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"

namespace Lust
{
	struct SSBOInstanceData
	{
		Eigen::Vector4<uint32_t> controllerInfo;
		Eigen::Vector4f texCoordsEdges;
		Eigen::Vector4<uint32_t> edgeColors;
	};
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
		Eigen::RowVector4f m_BaseVertices[2];
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
		static void BeginScene(const Camera& camera);
		/**
		* @brief Begins the scene
		* @param camera The camera pointer, is only used to access cameras provided by pointer
		*/
		static void BeginScene(const Camera* camera);
		/**
		* @brief Begins the scene
		* @param camera The camera
		*/
		static void BeginScene(const OrthographicCamera& camera);
		/**
		* @brief Begins the scene
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
		* @param ssboInstanceData The SSBO instance data
		*/
		static void DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, const SSBOInstanceData& ssboInstanceData);

		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param rotation The rotation of the quad
		* @param ssboInstanceData The SSBO instance data
		*/
		static void DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, float rotation, const SSBOInstanceData& ssboInstanceData);
		
		/**
		* @brief Draws a quad
		* @param model The model matrix
		* @param ssboInstanceData The SSBO instance data
		*/
		static void DrawQuad(const Eigen::Matrix4f& model, const SSBOInstanceData& ssboInstanceData);

		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param ssboInstanceData The SSBO instance data
		*/
		static void DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const SSBOInstanceData& ssboInstanceData);
		
		/**
		* @brief Draws a quad
		* @param position The position of the quad
		* @param size The size of the quad
		* @param rotation The rotation of the quad
		* @param ssboInstanceData The SSBO instance data
		*/
		static void DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, float rotation, const SSBOInstanceData& ssboInstanceData);

	private:
		/**
		* @brief Renders a quad
		* @param squareSmallBufferMatrix The matrix of the small buffer
		* @param color The color of the quad
		* @param element_names The element names of the small buffer
		* @param controllerInfo A set of 4 uint32_t values, the first one is the texture slot, the second one is the sampler slot, the third one is the level of detail, the fourth one is the mips bias
		*/
		static void RenderPush(const Eigen::Matrix4f& squareSmallBufferMatrix, const SSBOInstanceData& ssboInstanceData);

		/**
		* @brief Dispatches the draws
		*/
		static void DispatchDraws();

		/**
		* @brief Returns if the object is in the view frustum
		* @details Returns if the object is in the view frustum, comparing the vertices of the quad with the view frustum planes (-1.0f, 1.0f)[x,y]
		* @param squareSmallBufferMatrix The model matrix of the small buffer
		*/
		static bool ShouldRender(const Eigen::Matrix4f& squareSmallBufferMatrix);

		/**
		* @brief Returns if the object is in the view frustum
		* @details detects if the object is in the view frustum, comparing the vertices of the quad with the view frustum planes (-1.0f, 1.0f)[x,y]
		* @param vertices The vertices of the quad
		*/
		static bool IsInFrustum(const Eigen::RowVector4f* vertices);

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