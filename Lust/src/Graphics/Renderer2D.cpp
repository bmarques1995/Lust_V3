#include "Renderer2D.hpp"
#include <Application.hpp>
#include "RenderCommand.hpp"

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
			-.5f, -.5f, .2f,
			-.5f, .5f, .2f,
			.5f, -.5f, .2f,
			.5f, .5f, .2f
	};

	uint32_t squareIndices[6] = { 3,2,1, 1,2,0 };

	InputBufferLayout renderer2DLayout(
		{
			{ShaderDataType::Float3, "POSITION", false},
		});

	SmallBufferLayout renderer2DSmallBufferLayout(
		{
			//size_t offset, size_t size, uint32_t bindingSlot, uint32_t smallAttachment
			{ 0, 80, 0, context->GetSmallBufferAttachment() }
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	UniformLayout renderer2DUniformsLayout(
		{
			//BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t numberOfBuffers, uint32_t bufferAttachment, uint32_t bufferIndex, std::string name
			{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 1, 0, 1, AccessLevel::ROOT_BUFFER, 1, context->GetUniformAttachment(), 1, "m_CompleteMVP" } //
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);


	TextureLayout renderer2DTextureLayout(
		{
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);



	SamplerLayout renderer2DSamplerLayout(
		{
		}
		);

	size_t rows = 20, cols = 20;
	StructuredBufferLayout renderer2DStructuredBufferLayout(
		{
			//uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t bufferIndex, size_t stride, size_t numberOfBuffers, AccessLevel accessLevel, size_t bufferAlignment, std::string name
		}
	, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	InputInfo renderer2DInputInfoController(renderer2DLayout, renderer2DSmallBufferLayout, renderer2DUniformsLayout, renderer2DTextureLayout, renderer2DSamplerLayout, renderer2DStructuredBufferLayout);

	*s_SceneData = {
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity()
	};

	s_Renderer2DStorage->m_RawSmallBufferSize = renderer2DSmallBufferLayout.GetElement(0).GetSize();
	s_Renderer2DStorage->m_RawSmallBuffer = new uint8_t[s_Renderer2DStorage->m_RawSmallBufferSize];

	s_Renderer2DStorage->m_Shader.reset(Shader::Instantiate(context, "./assets/shaders/Renderer2D", renderer2DInputInfoController));
	s_Renderer2DStorage->m_VertexBuffer.reset(VertexBuffer::Instantiate(context, squareVertices, sizeof(squareVertices), renderer2DLayout.GetStride()));
	s_Renderer2DStorage->m_IndexBuffer.reset(IndexBuffer::Instantiate(context, squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	s_Renderer2DStorage->m_UniformBuffer.reset(UniformBuffer::Instantiate(context, (void*)s_SceneData.get(), sizeof(SceneData)));
	s_Renderer2DStorage->m_Shader->UploadConstantBuffer(&s_Renderer2DStorage->m_UniformBuffer, renderer2DUniformsLayout.GetElement("m_CompleteMVP"));
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

void Lust::Renderer2D::DrawQuad(const Eigen::Vector2f& position, const Eigen::Vector2f& size, const Eigen::Vector4f& color)
{
	Eigen::Transform<float, 3, Eigen::Affine, Eigen::ColMajor> element_transform = Eigen::Translation<float, 3>(position(0), position(1), 0.0f) * Eigen::Scaling(size(0), size(1), 1.0f);
	Eigen::Matrix4f squareSmallBufferMatrix = element_transform.matrix();

	s_Renderer2DStorage->m_UniformBuffer->Remap((void*)s_SceneData.get(), sizeof(SceneData));
	s_Renderer2DStorage->m_Shader->Stage();
	s_Renderer2DStorage->m_Shader->BindDescriptors();
	s_Renderer2DStorage->m_VertexBuffer->Stage();
	s_Renderer2DStorage->m_IndexBuffer->Stage();
	memcpy(&s_Renderer2DStorage->m_RawSmallBuffer[0], squareSmallBufferMatrix.data(), sizeof(squareSmallBufferMatrix));
	memcpy(&s_Renderer2DStorage->m_RawSmallBuffer[sizeof(squareSmallBufferMatrix)], color.data(), sizeof(color));
	s_Renderer2DStorage->m_Shader->BindSmallBuffer((void*)&s_Renderer2DStorage->m_RawSmallBuffer[0], s_Renderer2DStorage->m_RawSmallBufferSize, 0, 0);
	RenderCommand::DrawIndexed(s_Renderer2DStorage->m_IndexBuffer->GetCount(), 1);
}

void Lust::Renderer2D::DrawQuad(const Eigen::Vector3f& position, const Eigen::Vector2f& size, const Eigen::Vector4f& color)
{
	Eigen::Transform<float, 3, Eigen::Affine, Eigen::ColMajor> element_transform = Eigen::Translation<float, 3>(position) * Eigen::Scaling(size(0), size(1), 1.0f) ;
	Eigen::Matrix4f squareSmallBufferMatrix = element_transform.matrix();

	s_Renderer2DStorage->m_UniformBuffer->Remap((void*)s_SceneData.get(), sizeof(SceneData));
	s_Renderer2DStorage->m_Shader->Stage();
	s_Renderer2DStorage->m_Shader->BindDescriptors();
	s_Renderer2DStorage->m_VertexBuffer->Stage();
	s_Renderer2DStorage->m_IndexBuffer->Stage();
	memcpy(&s_Renderer2DStorage->m_RawSmallBuffer[0], squareSmallBufferMatrix.data(), sizeof(squareSmallBufferMatrix));
	memcpy(&s_Renderer2DStorage->m_RawSmallBuffer[sizeof(squareSmallBufferMatrix)], color.data(), sizeof(color));
	s_Renderer2DStorage->m_Shader->BindSmallBuffer((void*)&s_Renderer2DStorage->m_RawSmallBuffer[0], s_Renderer2DStorage->m_RawSmallBufferSize, 0, 0);
	RenderCommand::DrawIndexed(s_Renderer2DStorage->m_IndexBuffer->GetCount(), 1);
}
