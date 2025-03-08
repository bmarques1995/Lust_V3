#include "Renderer2D.hpp"
#include <Application.hpp>
#include "RenderCommand.hpp"
#include "MathBuffer.hpp"

std::shared_ptr<Lust::Renderer2DStorage> Lust::Renderer2D::s_Renderer2DStorage;
std::unique_ptr<Lust::SceneData> Lust::Renderer2D::s_SceneData;

void Lust::Renderer2D::Instantiate()
{
	auto window = Application::GetInstance()->GetWindow();
	auto context = Application::GetInstance()->GetContext();
	s_Renderer2DStorage = std::make_shared<Renderer2DStorage>();
	s_SceneData.reset(new SceneData);

	SmallMVP m_Renderer2DSmallMVP;

	float squareVertices[3 * 4] = {
			-.5f, -.5f, .0f,
			-.5f, .5f, .0f,
			.5f, -.5f, .0f,
			.5f, .5f, .0f
	};

	uint32_t squareIndices[6] = { 3,2,1, 1,2,0 };

	s_Renderer2DStorage->m_ShaderReflector.reset(ShaderReflector::Instantiate("./assets/shaders/Renderer2D", AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE));

	InputInfo renderer2DInputInfoController2(s_Renderer2DStorage->m_ShaderReflector->GetInputLayout(), s_Renderer2DStorage->m_ShaderReflector->GetSmallBufferLayout(), 
		s_Renderer2DStorage->m_ShaderReflector->GetUniformLayout(), s_Renderer2DStorage->m_ShaderReflector->GetTextureLayout(),
		s_Renderer2DStorage->m_ShaderReflector->GetSamplerLayout(), s_Renderer2DStorage->m_ShaderReflector->GetStructuredBufferLayout());

	*s_SceneData = {
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity()
	};

	s_Renderer2DStorage->m_RawSmallBufferSize = s_Renderer2DStorage->m_ShaderReflector->GetSmallBufferLayout().GetElement("m_SmallMVP").GetSize();
	s_Renderer2DStorage->m_RawSmallBuffer = new uint8_t[s_Renderer2DStorage->m_RawSmallBufferSize];

	s_Renderer2DStorage->m_Shader.reset(Shader::Instantiate(context, "./assets/shaders/Renderer2D", renderer2DInputInfoController2));
	s_Renderer2DStorage->m_VertexBuffer.reset(VertexBuffer::Instantiate(context, squareVertices, sizeof(squareVertices), s_Renderer2DStorage->m_Shader->GetInputLayout().GetStride()));
	s_Renderer2DStorage->m_IndexBuffer.reset(IndexBuffer::Instantiate(context, squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	s_Renderer2DStorage->m_UniformBuffer.reset(UniformBuffer::Instantiate(context, (void*)s_SceneData.get(), sizeof(SceneData)));
	s_Renderer2DStorage->m_Shader->UploadConstantBuffer(&s_Renderer2DStorage->m_UniformBuffer, s_Renderer2DStorage->m_Shader->GetUniformLayout().GetElement("m_CompleteMVP"));
}

void Lust::Renderer2D::Destroy()
{
	delete[] s_Renderer2DStorage->m_RawSmallBuffer;
	s_Renderer2DStorage->m_RawSmallBufferSize = 0;
	s_Renderer2DStorage->m_UniformBuffer.reset();
	s_Renderer2DStorage->m_IndexBuffer.reset();
	s_Renderer2DStorage->m_VertexBuffer.reset();
	s_Renderer2DStorage->m_Shader.reset();
	s_Renderer2DStorage.reset();
	s_SceneData.reset();
}

void Lust::Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	s_SceneData->view = camera.GetViewMatrix();
	s_SceneData->projection = camera.GetProjectionMatrix();
}

void Lust::Renderer2D::EndScene()
{
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, const Eigen::Vector4f& color, std::string_view element_name)
{
	DrawQuad(Eigen::Vector3f(position(0), position(1), 0.0f), size, color, element_name);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector4f& color, std::string_view element_name)
{
	Eigen::Transform<float, 3, Eigen::Affine, Eigen::ColMajor> element_transform = Eigen::Translation<float, 3>(position) * Eigen::Scaling(size(0), size(1), 1.0f) ;
	Eigen::Matrix4f squareSmallBufferMatrix = element_transform.matrix().transpose();

	s_Renderer2DStorage->m_UniformBuffer->Remap((void*)s_SceneData.get(), sizeof(SceneData));
	s_Renderer2DStorage->m_Shader->Stage();
	s_Renderer2DStorage->m_Shader->BindDescriptors();
	s_Renderer2DStorage->m_VertexBuffer->Stage();
	s_Renderer2DStorage->m_IndexBuffer->Stage();
	CopyMatrix4ToBuffer<float>(squareSmallBufferMatrix, &s_Renderer2DStorage->m_RawSmallBuffer, 0);
	memcpy(&s_Renderer2DStorage->m_RawSmallBuffer[sizeof(squareSmallBufferMatrix)], color.data(), sizeof(color));
	s_Renderer2DStorage->m_Shader->BindSmallBuffer((void*)&s_Renderer2DStorage->m_RawSmallBuffer[0], s_Renderer2DStorage->m_RawSmallBufferSize,
												   s_Renderer2DStorage->m_Shader->GetSmallBufferLayout().GetElement(element_name.data()), 0);
	RenderCommand::DrawIndexed(s_Renderer2DStorage->m_IndexBuffer->GetCount(), 1);
}
