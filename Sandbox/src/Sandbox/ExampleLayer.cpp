#include "ExampleLayer.hpp"
#include <imgui/imgui.h>
#include <Renderer.hpp>
#include <Operations.hpp>
#include <Application.hpp>
#include <RenderCommand.hpp>

Lust::ExampleLayer::ExampleLayer() :
	Layer("Example")
{
}

void Lust::ExampleLayer::OnAttach()
{
	auto window = Application::GetInstance()->GetWindow();
	auto context = Application::GetInstance()->GetContext();
	
	m_CompleteMVP = {
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity()
	};

	m_Camera.reset(new OrthographicCamera(window->GetWidth() * -.5f, window->GetWidth() * .5f, window->GetHeight() * -.5f, window->GetHeight() * .5f));
	InputBufferLayout layout(
		{
			{ShaderDataType::Float3, "POSITION", false},
			{ShaderDataType::Float4, "COLOR", false},
			{ShaderDataType::Float2, "TEXCOORD", false},
		});

	SmallBufferLayout smallBufferLayout(
		{
			//size_t offset, size_t size, uint32_t bindingSlot, uint32_t smallAttachment
			{ 0, 64, 0, (*context)->GetSmallBufferAttachment() }
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	UniformLayout uniformsLayout(
		{
			//BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t numberOfBuffers, uint32_t bufferAttachment, uint32_t bufferIndex
			{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 1, 0, 1, AccessLevel::ROOT_BUFFER, 1, (*context)->GetUniformAttachment(), 1 }, //
			{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 2, 0, 2, AccessLevel::ROOT_BUFFER, 1, (*context)->GetUniformAttachment(), 1 } //
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	m_Texture1.reset(Texture2D::Instantiate(context, "./assets/textures/yor.png", 3, 0, 3, 0));
	m_Texture2.reset(Texture2D::Instantiate(context, "./assets/textures/sample.png", 4, 0, 3, 1));

	m_SmallMVP.model = Scale<float>(Eigen::Matrix4f::Identity(), Eigen::Vector<float, 3>(m_Texture1->GetWidth() * .5f, m_Texture1->GetHeight() * .5f, 1.0f));

	TextureLayout textureLayout(
		{
			m_Texture1->GetTextureDescription(),
			m_Texture2->GetTextureDescription(),
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);



	SamplerLayout samplerLayout(
		{
			//SamplerFilter filter, AnisotropicFactor anisotropicFactor, AddressMode addressMode, ComparisonPassMode comparisonPassMode, uint32_t bindingSlot, uint32_t shaderRegister, uint32_t samplerIndex
			{SamplerFilter::LINEAR, AnisotropicFactor::FACTOR_4, AddressMode::BORDER, ComparisonPassMode::ALWAYS, 5, 0, 4, 0},
			{SamplerFilter::NEAREST, AnisotropicFactor::FACTOR_4, AddressMode::BORDER, ComparisonPassMode::ALWAYS, 6, 0, 4, 1},
		}
		);

	InputInfo inputInfoController(layout, smallBufferLayout, uniformsLayout, textureLayout, samplerLayout);

	m_Shader.reset(Shader::Instantiate(context, "./assets/shaders/HelloTriangle", inputInfoController));

	m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), uniformsLayout.GetElement(1));
	m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), uniformsLayout.GetElement(2));
	m_Shader->UploadTexture2D(&m_Texture1);
	m_Shader->UploadTexture2D(&m_Texture2);
	m_VertexBuffer.reset(VertexBuffer::Instantiate(context, (const void*)&m_VBuffer[0], sizeof(m_VBuffer), layout.GetStride()));
	m_IndexBuffer.reset(IndexBuffer::Instantiate(context, (const void*)&iBuffer[0], sizeof(iBuffer) / sizeof(uint32_t)));
	
}

void Lust::ExampleLayer::OnDetach()
{
	m_Texture2.reset();
	m_Texture1.reset();
	m_IndexBuffer.reset();
	m_VertexBuffer.reset();
	m_Shader.reset();
}

void Lust::ExampleLayer::OnUpdate(Timestep ts)
{
	if (Input::IsKeyPressed(Key::LUST_KEYCODE_ESCAPE))
		Console::CoreDebug("Escape key pressed");
	if (Input::IsMouseButtonPressed(Mouse::LUST_BUTTON_LEFT))
		Console::CoreDebug("({}, {})", Input::GetMousePosition().first, Input::GetMousePosition().second);
	if (Input::IsGamepadKeyPressed(Gamepad::LUST_GAMEPAD_BUTTON_SOUTH))
		Console::CoreDebug("A/Cross button pressed");
	int16_t leftStickX = Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTX);
	int16_t leftStickY = Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTY);
	if ((leftStickX > 980) && (leftStickY > 980))
		Console::CoreDebug("Left joystick moved ({},{})", leftStickX, leftStickY);

	Renderer::BeginScene(*(m_Camera.get()));
	Renderer::SubmitCBV(m_Shader, m_Shader->GetUniformLayout().GetElement(1));
	Renderer::SubmitCBV(m_Shader, m_Shader->GetUniformLayout().GetElement(2));
	Renderer::SubmitShader(m_Shader, m_VertexBuffer, m_IndexBuffer, m_SmallMVP.model);
	Renderer::EndScene();
	RenderCommand::DrawIndexed(m_IndexBuffer->GetCount());
}

void Lust::ExampleLayer::OnImGuiRender()
{
	if (m_ShowDemoWindow)
		ImGui::ShowDemoWindow(&m_ShowDemoWindow);
}

void Lust::ExampleLayer::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>(std::bind(&ExampleLayer::OnWindowResize, this, std::placeholders::_1));
}

bool Lust::ExampleLayer::OnWindowResize(WindowResizeEvent& e)
{
	m_Camera->SetProjection(e.GetWidth() * -.5f, e.GetWidth() *.5f, e.GetHeight() * -.5f, e.GetHeight() * .5f);
	return true;
}


