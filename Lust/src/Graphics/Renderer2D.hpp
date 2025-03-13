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
	struct Renderer2DStorage
	{
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<ShaderReflector> m_ShaderReflector;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<UniformBuffer> m_UniformBuffer;
		std::shared_ptr<Texture2D> m_Texture;
		std::shared_ptr<Sampler> m_Sampler;
		std::shared_ptr<Texture2D> m_WhiteTexture;
		std::shared_ptr<Sampler> m_WhiteSampler;
		uint8_t *m_RawSmallBuffer;
		size_t m_RawSmallBufferSize;
	};
	class LUST_API Renderer2D
	{
	public:
		static void Instantiate();
		static void Destroy();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void UploadTexture2D(const std::shared_ptr<Texture2D>& texture, const std::shared_ptr<Sampler>& sampler);

		static void DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, const Eigen::Vector3f& color, std::string_view element_name);
		static void DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector3f& color, std::string_view element_name);
		static void DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector4f& color, std::string_view element_name);

		static void DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, std::string_view element_name);
		static void DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, std::string_view element_name);
	private:
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