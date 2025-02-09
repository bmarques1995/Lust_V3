#pragma once

#include "OrthographicCamera.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include <memory>

namespace Lust
{
	struct SceneData
	{
		Eigen::Matrix4f model;
		Eigen::Matrix4f view;
		Eigen::Matrix4f projection;
		Eigen::Matrix4f mipLevel;
	};

	class LUST_API Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void SubmitCBV(const std::shared_ptr<Shader>& shader, const UniformElement& uploadCBV);
		static void SubmitShader(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer,  const Eigen::Matrix4f& transform = Eigen::Matrix4f::Identity());

	private:
		

		static std::unique_ptr<SceneData> s_SceneData;
	};
}