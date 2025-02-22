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

		static void SubmitSmallBuffer(const std::shared_ptr<Shader>& shader, const void* data, size_t size, uint32_t bindingSlot, size_t offset = 0);
		static void SubmitCBV(std::shared_ptr<UniformBuffer>* uniformBuffer);
		static void SubmitShader(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer);

	private:
		

		static std::unique_ptr<SceneData> s_SceneData;
	};
}