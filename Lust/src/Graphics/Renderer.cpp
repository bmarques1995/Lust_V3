#include "Renderer.hpp"

std::unique_ptr<Lust::SceneData> Lust::Renderer::s_SceneData = nullptr;

void Lust::Renderer::Init()
{
	s_SceneData = std::make_unique<SceneData>();
	*s_SceneData = {
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity()
	};
}

void Lust::Renderer::Shutdown()
{
	s_SceneData.reset();
}

void Lust::Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
}

void Lust::Renderer::BeginScene(OrthographicCamera& camera)
{
	s_SceneData->view = camera.GetViewMatrix();
	s_SceneData->projection = camera.GetProjectionMatrix();
}

void Lust::Renderer::EndScene()
{
}

void Lust::Renderer::SubmitCBV(const std::shared_ptr<Shader>& shader, const UniformElement& uploadCBV)
{
	shader->UpdateCBuffer((void *)s_SceneData.get(), sizeof(SceneData), uploadCBV);
}

void Lust::Renderer::SubmitShader(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer, const Eigen::Matrix4f& transform)
{
	shader->Stage();
	shader->BindSmallBuffer(transform.data(), sizeof(transform), 0);
	shader->BindDescriptors();
	vertexBuffer->Stage();
	indexBuffer->Stage();
}

