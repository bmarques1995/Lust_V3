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

	*s_SceneData = {
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity()
	};

	s_Renderer2DStorage->m_ShaderReflector->GetStructuredBufferLayout().GetElement("u_InstancedMVP").SetNumberOfElements(s_Renderer2DStorage->c_MaxInstanceCount);

	size_t ssboSize = s_Renderer2DStorage->m_ShaderReflector->GetStructuredBufferLayout().GetElement("u_InstancedMVP").GetSize();
	uint8_t* voidSSBO = new uint8_t[ssboSize];

	s_Renderer2DStorage->m_StructuredBuffer.reset(StructuredBuffer::Instantiate(context, voidSSBO, ssboSize));
	s_Renderer2DStorage->m_SSBOInstanceBufferSize = s_Renderer2DStorage->m_ShaderReflector->GetStructuredBufferLayout().GetElement("u_InstancedMVP").GetStride();
	s_Renderer2DStorage->m_SSBOInstanceBuffer = new uint8_t[s_Renderer2DStorage->m_SSBOInstanceBufferSize];

	delete[] voidSSBO;

	s_Renderer2DStorage->m_Shader.reset(Shader::Instantiate(context, "./assets/shaders/Renderer2D", s_Renderer2DStorage->m_ShaderReflector));
	s_Renderer2DStorage->m_VertexBuffer.reset(VertexBuffer::Instantiate(context, squareVertices, sizeof(squareVertices), s_Renderer2DStorage->m_Shader->GetInputLayout().GetStride()));
	s_Renderer2DStorage->m_IndexBuffer.reset(IndexBuffer::Instantiate(context, squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	s_Renderer2DStorage->m_UniformBuffer.reset(UniformBuffer::Instantiate(context, (void*)s_SceneData.get(), sizeof(SceneData)));
	s_Renderer2DStorage->m_Shader->UploadConstantBuffer(&s_Renderer2DStorage->m_UniformBuffer, s_Renderer2DStorage->m_Shader->GetUniformLayout().GetElement("m_CompleteMVP"));
	s_Renderer2DStorage->m_Shader->UploadStructuredBuffer(&s_Renderer2DStorage->m_StructuredBuffer, s_Renderer2DStorage->m_Shader->GetStructuredBufferLayout().GetElement("u_InstancedMVP"));

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
	delete[] s_Renderer2DStorage->m_SSBOInstanceBuffer;
	s_Renderer2DStorage->m_WhiteSampler.reset();
	s_Renderer2DStorage->m_WhiteTexture.reset();
	s_Renderer2DStorage->m_SSBOInstanceBufferSize = 0;
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
	s_Renderer2DStorage->m_InstanceCount = 0;
}

void Lust::Renderer2D::BeginScene(const OrthographicCamera* camera)
{
	BeginScene(*camera);
}

void Lust::Renderer2D::EndScene()
{
	DispatchDraws();
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

void Lust::Renderer2D::DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, const SSBOInstanceData& ssboInstanceData)
{
	DrawQuad(position, size, 0.0f, ssboInstanceData);
}
void Lust::Renderer2D::DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, float rotation, const SSBOInstanceData& ssboInstanceData)
{
	DrawQuad(Eigen::Vector3f(position(0), position(1), 0.0f), size, rotation, ssboInstanceData);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Matrix4f& model, const SSBOInstanceData& ssboInstanceData)
{
	RenderPush(model, ssboInstanceData);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const SSBOInstanceData& ssboInstanceData)
{
	DrawQuad(position, size, 0.0f, ssboInstanceData);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, float rotation, const SSBOInstanceData& ssboInstanceData)
{
	Eigen::Quaternionf q(Eigen::AngleAxisf(rotation, Eigen::Vector3f(0.0f, 0.0f, 1.0f)));
	Eigen::Transform<float, 3, Eigen::Affine, Eigen::ColMajor> element_transform = Eigen::Translation<float, 3>(position) * Eigen::Scaling(size(0), size(1), 1.0f) * q;
	Eigen::Matrix4f squareSmallBufferMatrix = element_transform.matrix().transpose();

	RenderPush(squareSmallBufferMatrix, ssboInstanceData);
}

void Lust::Renderer2D::RenderPush(const Eigen::Matrix4f& squareSmallBufferMatrix, const SSBOInstanceData& ssboInstanceData)
{
	if (s_Renderer2DStorage->m_InstanceCount >= s_Renderer2DStorage->c_MaxInstanceCount)
	{
		DispatchDraws();
		s_Renderer2DStorage->m_InstanceCount = 0;
	}
	size_t bufferOffset = 0;
	CopyMatrix4ToBuffer<float>(squareSmallBufferMatrix, &s_Renderer2DStorage->m_SSBOInstanceBuffer, 0);
	bufferOffset += sizeof(squareSmallBufferMatrix);
	memcpy(&s_Renderer2DStorage->m_SSBOInstanceBuffer[bufferOffset], ssboInstanceData.controllerInfo.data(), sizeof(ssboInstanceData.controllerInfo));
	bufferOffset += sizeof(ssboInstanceData.controllerInfo);
	memcpy(&s_Renderer2DStorage->m_SSBOInstanceBuffer[bufferOffset], ssboInstanceData.texCoordsEdges.data(), sizeof(ssboInstanceData.texCoordsEdges));
	bufferOffset += sizeof(ssboInstanceData.texCoordsEdges);
	memcpy(&s_Renderer2DStorage->m_SSBOInstanceBuffer[bufferOffset], ssboInstanceData.edgeColors.data(), sizeof(ssboInstanceData.edgeColors));
	s_Renderer2DStorage->m_StructuredBuffer->Remap(s_Renderer2DStorage->m_SSBOInstanceBuffer, s_Renderer2DStorage->m_SSBOInstanceBufferSize,
		s_Renderer2DStorage->m_SSBOInstanceBufferSize * s_Renderer2DStorage->m_InstanceCount);
	s_Renderer2DStorage->m_InstanceCount++;
}

void Lust::Renderer2D::DispatchDraws()
{
	s_Renderer2DStorage->m_UniformBuffer->Remap((void*)s_SceneData.get(), sizeof(SceneData));
	s_Renderer2DStorage->m_Shader->Stage();
	s_Renderer2DStorage->m_Shader->BindDescriptors();
	s_Renderer2DStorage->m_VertexBuffer->Stage();
	s_Renderer2DStorage->m_IndexBuffer->Stage();
	RenderCommand::DrawIndexed(s_Renderer2DStorage->m_IndexBuffer->GetCount(), s_Renderer2DStorage->m_InstanceCount);
}
