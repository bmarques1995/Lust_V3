#include "SandCoffin2D.hpp"
#include <Application.hpp>
#include <imgui/imgui.h>
#include <RenderCommand.hpp>

Lust::SandCoffin2D::SandCoffin2D() :
	Layer("SandCoffin2D")
{

}

Lust::SandCoffin2D::~SandCoffin2D()
{

}

void Lust::SandCoffin2D::OnAttach()
{
	auto window = Application::GetInstance()->GetWindow();
	auto context = Application::GetInstance()->GetContext();

	m_CameraController.reset(new OrthographicCameraController(window->GetWidth(), window->GetHeight(), true));
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

	m_Renderer2DCompleteMVP = {
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity()
	};

	m_Renderer2DRawSmallBufferSize = renderer2DSmallBufferLayout.GetElement(0).GetSize();
	m_Renderer2DRawSmallBuffer = new uint8_t[m_Renderer2DRawSmallBufferSize];

	m_Renderer2DShader.reset(Shader::Instantiate(context, "./assets/shaders/Renderer2D", renderer2DInputInfoController));
	m_Renderer2DVertexBuffer.reset(VertexBuffer::Instantiate(context, squareVertices, sizeof(squareVertices), renderer2DLayout.GetStride()));
	m_Renderer2DIndexBuffer.reset(IndexBuffer::Instantiate(context, squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	m_Renderer2DUniformBuffer.reset(UniformBuffer::Instantiate(context, &(m_Renderer2DCompleteMVP.model(0,0)), sizeof(m_Renderer2DCompleteMVP)));
	m_Renderer2DShader->UploadConstantBuffer(&m_Renderer2DUniformBuffer, renderer2DUniformsLayout.GetElement("m_CompleteMVP"));
}

void Lust::SandCoffin2D::OnDetach()
{
	delete[] m_Renderer2DRawSmallBuffer;
	m_Renderer2DRawSmallBufferSize = 0;
	m_CameraController.reset();
	m_Renderer2DUniformBuffer.reset();
	m_Renderer2DIndexBuffer.reset();
	m_Renderer2DVertexBuffer.reset();
	m_Renderer2DShader.reset();
}

void Lust::SandCoffin2D::OnUpdate(Timestep ts)
{
	m_CameraController->OnUpdate(ts);

	Renderer::BeginScene(m_CameraController->GetCamera());
	Eigen::Transform<float, 3, Eigen::Affine> element_transform = Eigen::Translation<float, 3>(0.0f, 0.0f, 0.0f) * Eigen::Scaling(350.0f, 350.0f, 1.0f);
	Eigen::Matrix4f squareSmallBufferMatrix = element_transform.matrix();

	Renderer::SubmitCBV(&m_Renderer2DUniformBuffer);
	Renderer::SubmitShader(m_Renderer2DShader, m_Renderer2DVertexBuffer, m_Renderer2DIndexBuffer);
	memcpy(&m_Renderer2DRawSmallBuffer[0], squareSmallBufferMatrix.data(), sizeof(squareSmallBufferMatrix));
	memcpy(&m_Renderer2DRawSmallBuffer[sizeof(m_Renderer2DSmallMVP.model)], m_Renderer2DColor.data(), sizeof(m_Renderer2DColor));
	Renderer::SubmitSmallBuffer(m_Renderer2DShader, (void*)&m_Renderer2DRawSmallBuffer[0], m_Renderer2DRawSmallBufferSize, 0);
	RenderCommand::DrawIndexed(m_Renderer2DIndexBuffer->GetCount(), 1);

	Renderer::EndScene();
}

void Lust::SandCoffin2D::OnImGuiRender()
{
	ImGui::Begin("Color Editor");
	ImGui::ColorEdit4("Square Color", &m_Renderer2DColor(0));
	ImGui::End();
}

void Lust::SandCoffin2D::OnEvent(Event& event)
{
	m_CameraController->OnEvent(event);
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizedEvent>(std::bind(&SandCoffin2D::OnWindowResize, this, std::placeholders::_1), true);
}

bool Lust::SandCoffin2D::OnWindowResize(WindowResizedEvent& e)
{
	return false;
}
