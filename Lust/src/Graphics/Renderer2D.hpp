#pragma once

#include "LustDLLMacro.hpp"
#include "OrthographicCamera.hpp"
#include <Eigen/Eigen>
#include "Shader.hpp"
#include "Buffer.hpp"

namespace Lust
{
	class LUST_API Renderer2D
	{
	public:
		static void Instantiate();
		static void Destroy();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, const Eigen::Vector4f& color);
		static void DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector4f& color);
	private:
		struct Renderer2DStorage
		{
			std::shared_ptr<Shader> m_Shader;
			std::shared_ptr<VertexBuffer> m_VertexBuffer;
			std::shared_ptr<IndexBuffer> m_IndexBuffer;
		};

		static std::shared_ptr<Renderer2DStorage> m_Renderer2DStorage;
	};
}