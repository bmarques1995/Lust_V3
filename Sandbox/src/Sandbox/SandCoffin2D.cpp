#include "SandCoffin2D.hpp"
#include "MathBuffer.hpp"
#include <Application.hpp>
#include <imgui/imgui.h>
#include <RenderCommand.hpp>
#include <Renderer2D.hpp>

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

	InputBufferLayout layout(
		{
			{ShaderDataType::Float3, "POSITION", false},
			{ShaderDataType::Float2, "TEXCOORD", false},
		});

	SmallBufferLayout smallBufferLayout({
			{ 0, 80, 0, context->GetSmallBufferAttachment(), "m_SmallMVP" }
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	UniformLayout uniformsLayout(
		{
			//BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t numberOfBuffers, uint32_t bufferAttachment, uint32_t bufferIndex
			{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 1, 0, 1, AccessLevel::ROOT_BUFFER, 1, context->GetUniformAttachment(), 1, "m_CompleteMVP" },
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	TextureLayout textureLayout(
		{
			//uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t textureIndex, std::string name
			{ 2, 0, 2, 0, "textureChecker" },
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	SamplerLayout samplerLayout(
		{
			//SamplerFilter filter, AnisotropicFactor anisotropicFactor, AddressMode addressMode, ComparisonPassMode comparisonPassMode, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t samplerIndex
			{ SamplerFilter::NEAREST, AnisotropicFactor::FACTOR_4, AddressMode::REPEAT, ComparisonPassMode::ALWAYS, 3, 0, 3, 0, "dynamicSampler" },
		});

	StructuredBufferLayout structuredBufferLayout({

		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	InputInfo inputInfoController(layout, smallBufferLayout, uniformsLayout, textureLayout, samplerLayout, structuredBufferLayout);

	m_Renderer2DShaderReflector.reset(ShaderReflector::Instantiate("./assets/shaders/TexturedRenderer2D"));
	m_Renderer2DShader.reset(Shader::Instantiate(context, "./assets/shaders/TexturedRenderer2D", inputInfoController));
	m_Renderer2DTexture.reset(Texture2D::Instantiate(context, "./assets/textures/sample.png"));
	
	m_Renderer2DVertexBuffer.reset(VertexBuffer::Instantiate(context, (const void*)&squareVertices[0], sizeof(squareVertices), layout.GetStride()));
	m_Renderer2DIndexBuffer.reset(IndexBuffer::Instantiate(context, (const uint32_t*)&squareIndices[0], sizeof(squareIndices) / sizeof(uint32_t)));
	m_Renderer2DUniformBuffer.reset(UniformBuffer::Instantiate(context, (const void*)&m_Renderer2DCompleteMVP.model(0,0), sizeof(m_Renderer2DCompleteMVP)));
	m_Renderer2DRawSmallBufferSize = smallBufferLayout.GetElement("m_SmallMVP").GetSize();
	m_Renderer2DRawSmallBuffer = new uint8_t[m_Renderer2DRawSmallBufferSize];

	auto textureElement = m_Renderer2DShader->GetTextureElements().find("textureChecker");
	if (textureElement != m_Renderer2DShader->GetTextureElements().end())
		m_Renderer2DShader->UploadTexture2D(&m_Renderer2DTexture, textureElement->second);
	else
		Lust::Console::Assert(false, "Could not find texture element in shader: {}", m_Renderer2DShader->GetName());

	auto uniformElement = m_Renderer2DShader->GetUniformElements().find("m_CompleteMVP");
	if (uniformElement != m_Renderer2DShader->GetUniformElements().end())
		m_Renderer2DShader->UploadConstantBuffer(&m_Renderer2DUniformBuffer, uniformElement->second);
	else
		Lust::Console::Assert(false, "Could not find uniform element in shader: {}", m_Renderer2DShader->GetName());
}

void Lust::SandCoffin2D::OnDetach()
{
	delete[] m_Renderer2DRawSmallBuffer;
	m_Renderer2DTexture.reset();
	m_Renderer2DVertexBuffer.reset();
	m_Renderer2DIndexBuffer.reset();
	m_Renderer2DUniformBuffer.reset();
	m_Renderer2DShader.reset();
	m_CameraController.reset();
}

void Lust::SandCoffin2D::OnUpdate(Timestep ts)
{
	m_CameraController->OnUpdate(ts);

	Renderer2D::BeginScene(m_CameraController->GetCamera());
	Renderer2D::DrawQuad(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector2f(150.0f, 150.0f), m_Renderer2DColor, "m_SmallMVP");
	Renderer2D::DrawQuad(Eigen::Vector3f(450.0f, 200.0f, 0.0f), Eigen::Vector2f(100.0f, 130.0f), Eigen::Vector4f(.8f, .2f, .3f, 1.0f), "m_SmallMVP");
	Renderer2D::EndScene();

	Renderer::BeginScene(m_CameraController->GetCamera());
	Eigen::Transform<float, 3, Eigen::Affine, Eigen::ColMajor> element_transform = Eigen::Translation<float, 3>(Eigen::Vector3f(0.0f, 0.0f, 0.0f)) * Eigen::Scaling(2048.0f, 2048.0f, 1.0f);
	Eigen::Matrix4f squareSmallBufferMatrix = element_transform.matrix().transpose();

	Renderer::SubmitCBV(&m_Renderer2DUniformBuffer);
	Renderer::SubmitShader(m_Renderer2DShader, m_Renderer2DVertexBuffer, m_Renderer2DIndexBuffer);
	CopyMatrix4ToBuffer<float>(squareSmallBufferMatrix, &m_Renderer2DRawSmallBuffer, 0);
	memcpy(&m_Renderer2DRawSmallBuffer[sizeof(squareSmallBufferMatrix)], m_Renderer2DColor.data(), sizeof(m_Renderer2DColor));
	Renderer::SubmitSmallBuffer(m_Renderer2DShader, (void*)&m_Renderer2DRawSmallBuffer[0], m_Renderer2DRawSmallBufferSize, m_Renderer2DShader->GetSmallBufferLayout().GetElement("m_SmallMVP"));
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
