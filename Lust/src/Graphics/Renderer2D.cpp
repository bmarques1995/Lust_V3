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

	float squareVertices[5 * 4] = {
			-.5f, -.5f, .0f, 0.0f, 1.0f,
			-.5f, .5f, .0f, 0.0f, 0.0f,
			.5f, -.5f, .0f, 1.0f, 1.0f,
			.5f, .5f, .0f, 1.0f, 0.0f
	};

	uint32_t squareIndices[6] = { 3,2,1, 1,2,0 };

	s_Renderer2DStorage->m_ShaderReflector.reset(ShaderReflector::Instantiate("./assets/shaders/Renderer2D", AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE));

	InputInfo renderer2DInputInfoController2(s_Renderer2DStorage->m_ShaderReflector->GetInputLayout(), s_Renderer2DStorage->m_ShaderReflector->GetSmallBufferLayout(), 
		s_Renderer2DStorage->m_ShaderReflector->GetUniformLayout(), s_Renderer2DStorage->m_ShaderReflector->GetTextureLayout(),
		s_Renderer2DStorage->m_ShaderReflector->GetSamplerLayout(), s_Renderer2DStorage->m_ShaderReflector->GetTextureArrayLayout(),
		s_Renderer2DStorage->m_ShaderReflector->GetSamplerArrayLayout(), s_Renderer2DStorage->m_ShaderReflector->GetStructuredBufferLayout());

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

	Lust::SamplerInfo dynamicSamplerInfo2(Lust::SamplerFilter::NEAREST, Lust::AnisotropicFactor::FACTOR_4, Lust::AddressMode::REPEAT, Lust::ComparisonPassMode::ALWAYS);
	s_Renderer2DStorage->m_WhiteSampler.reset(Sampler::Instantiate(context, dynamicSamplerInfo2));
	s_Renderer2DStorage->m_WhiteTexture.reset(Texture2D::Instantiate(context, 1, 1, "WhiteTexture"));
	auto textureArray = s_Renderer2DStorage->m_Shader->GetTextureArrayLayout().GetElement("renderTexture");
	auto samplerArray = s_Renderer2DStorage->m_Shader->GetSamplerArrayLayout().GetElement("dynamicSampler");
	for (size_t i = 0; i < textureArray.GetNumberOfTextures(); i++)
	{
		s_Renderer2DStorage->m_Shader->UploadTexture2D(&s_Renderer2DStorage->m_WhiteTexture, textureArray, i);
	}

	for (size_t i = 0; i < samplerArray.GetNumberOfSamplers(); i++)
	{
		s_Renderer2DStorage->m_Shader->UploadSampler(&s_Renderer2DStorage->m_WhiteSampler, samplerArray, i);
	}
	s_Renderer2DStorage->m_Shader->UploadTexturePackedDescSet(textureArray);
	s_Renderer2DStorage->m_Shader->UploadSamplerPackedDescSet(samplerArray);
}

void Lust::Renderer2D::Destroy()
{
	delete[] s_Renderer2DStorage->m_RawSmallBuffer;
	s_Renderer2DStorage->m_WhiteSampler.reset();
	s_Renderer2DStorage->m_WhiteTexture.reset();
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

void Lust::Renderer2D::BeginScene(const OrthographicCamera* camera)
{
	BeginScene(*camera);
}

void Lust::Renderer2D::EndScene()
{
}

void Lust::Renderer2D::UploadTexture2D(const std::shared_ptr<Texture2D>& texture, uint32_t textureSlot)
{
	auto textureArray = s_Renderer2DStorage->m_Shader->GetTextureArrayLayout().GetElement("renderTexture");
	s_Renderer2DStorage->m_Shader->UploadTexture2D(&texture, textureArray, textureSlot);
	s_Renderer2DStorage->m_Shader->UploadTexturePackedDescSet(textureArray);
}

void Lust::Renderer2D::UploadSampler(const std::shared_ptr<Sampler>& sampler, uint32_t samplerSlot)
{
	auto samplerArray = s_Renderer2DStorage->m_Shader->GetSamplerArrayLayout().GetElement("dynamicSampler");
	s_Renderer2DStorage->m_Shader->UploadSampler(&sampler, samplerArray, samplerSlot);
	s_Renderer2DStorage->m_Shader->UploadSamplerPackedDescSet(samplerArray);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, const Eigen::Vector3f& color, std::string_view element_name)
{
	
	DrawQuad(Eigen::Vector3f(position(0), position(1), 0.0f), size, color, element_name);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, const Eigen::Vector3f& color, float rotation, std::string_view element_name)
{
	DrawQuad(Eigen::Vector3f(position(0), position(1), 0.0f), size, color, rotation, element_name);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector3f& color, std::string_view element_name)
{
	Eigen::Vector4<uint32_t> controllerInfo = Eigen::Vector4<uint32_t>(0, 0, 0, 0);
	Eigen::Vector4f finalColor = Eigen::Vector4f(color(0), color(1), color(2), 0.0f);
	DrawQuad(position, size, finalColor, 0.0f, controllerInfo, element_name);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector3f& color, float rotation, std::string_view element_name)
{
	Eigen::Vector4<uint32_t> controllerInfo = Eigen::Vector4<uint32_t>(0, 0, 0, 0);
	Eigen::Vector4f finalColor = Eigen::Vector4f(color(0), color(1), color(2), 0.0f);
	DrawQuad(position, size, finalColor, rotation, controllerInfo, element_name);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector4f& color, const Eigen::Vector4<uint32_t>& controllerInfo, std::string_view element_name)
{
	DrawQuad(position, size, color, 0.0f, controllerInfo, element_name);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector4f& color, float rotation, const Eigen::Vector4<uint32_t>& controllerInfo, std::string_view element_name)
{
	Eigen::Quaternionf q(Eigen::AngleAxisf(rotation, Eigen::Vector3f(0.0f, 0.0f, 1.0f)));
	Eigen::Transform<float, 3, Eigen::Affine, Eigen::ColMajor> element_transform = Eigen::Translation<float, 3>(position) * Eigen::Scaling(size(0), size(1), 1.0f) * q;
	Eigen::Matrix4f squareSmallBufferMatrix = element_transform.matrix().transpose();

	RenderAction(squareSmallBufferMatrix, color, element_name, controllerInfo);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, float tilingFactor, const Eigen::Vector4<uint32_t>& controllerInfo, std::string_view element_name)
{
	DrawQuad(Eigen::Vector3f(position(0), position(1), 0.0f), size, tilingFactor, controllerInfo, element_name);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, float tilingFactor, float rotation, const Eigen::Vector4<uint32_t>& controllerInfo, std::string_view element_name)
{
	DrawQuad(Eigen::Vector3f(position(0), position(1), 0.0f), size, tilingFactor, rotation, controllerInfo, element_name);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, float tilingFactor, const Eigen::Vector4<uint32_t>& controllerInfo, std::string_view element_name)
{
	Eigen::Vector4f color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, tilingFactor);

	DrawQuad(position, size, color, 0.0f, controllerInfo, element_name);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, float tilingFactor, float rotation, const Eigen::Vector4<uint32_t>& controllerInfo, std::string_view element_name)
{
	Eigen::Vector4f color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, tilingFactor);

	DrawQuad(position, size, color, rotation, controllerInfo, element_name);
}

void Lust::Renderer2D::RenderAction(const Eigen::Matrix4f& squareSmallBufferMatrix, const Eigen::Vector4f& color, std::string_view element_name, const Eigen::Vector4<uint32_t>& controllerInfo)
{
	s_Renderer2DStorage->m_UniformBuffer->Remap((void*)s_SceneData.get(), sizeof(SceneData));
	s_Renderer2DStorage->m_Shader->Stage();
	s_Renderer2DStorage->m_Shader->BindDescriptors();
	s_Renderer2DStorage->m_VertexBuffer->Stage();
	s_Renderer2DStorage->m_IndexBuffer->Stage();
	CopyMatrix4ToBuffer<float>(squareSmallBufferMatrix, &s_Renderer2DStorage->m_RawSmallBuffer, 0);
	memcpy(&s_Renderer2DStorage->m_RawSmallBuffer[sizeof(squareSmallBufferMatrix)], color.data(), sizeof(color));
	memcpy(&s_Renderer2DStorage->m_RawSmallBuffer[sizeof(squareSmallBufferMatrix) + sizeof(color)], controllerInfo.data(), sizeof(controllerInfo));
	s_Renderer2DStorage->m_Shader->BindSmallBuffer((void*)&s_Renderer2DStorage->m_RawSmallBuffer[0], s_Renderer2DStorage->m_RawSmallBufferSize,
		s_Renderer2DStorage->m_Shader->GetSmallBufferLayout().GetElement(element_name.data()), 0);
	RenderCommand::DrawIndexed(s_Renderer2DStorage->m_IndexBuffer->GetCount(), 1);
}
