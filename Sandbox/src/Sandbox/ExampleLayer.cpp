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
			{ 0, 64, 0, context->GetSmallBufferAttachment() }
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	UniformLayout uniformsLayout(
		{
			//BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t numberOfBuffers, uint32_t bufferAttachment, uint32_t bufferIndex
			{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 1, 0, 1, AccessLevel::ROOT_BUFFER, 1, context->GetUniformAttachment(), 1 }, //
			{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 2, 0, 2, AccessLevel::ROOT_BUFFER, 1, context->GetUniformAttachment(), 1 } //
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

	StructuredBufferLayout structuredBufferLayout(
		{
		}
		, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	InputInfo inputInfoController(layout, smallBufferLayout, uniformsLayout, textureLayout, samplerLayout, structuredBufferLayout);

	m_Shader.reset(Shader::Instantiate(context, "./assets/shaders/HelloTriangle", inputInfoController));

	m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), uniformsLayout.GetElement(1));
	m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), uniformsLayout.GetElement(2));
	m_Shader->UploadTexture2D(&m_Texture1);
	m_Shader->UploadTexture2D(&m_Texture2);
	m_VertexBuffer.reset(VertexBuffer::Instantiate(context, (const void*)&m_VBuffer[0], sizeof(m_VBuffer), layout.GetStride()));
	m_IndexBuffer.reset(IndexBuffer::Instantiate(context, (const void*)&m_IBuffer[0], sizeof(m_IBuffer) / sizeof(uint32_t)));
	
	InputBufferLayout squareLayout(
		{
			{ShaderDataType::Float3, "POSITION", false},
			{ShaderDataType::Float2, "TEXCOORD", false},
		});

	SmallBufferLayout squareSmallBufferLayout(
		{
			//size_t offset, size_t size, uint32_t bindingSlot, uint32_t smallAttachment
			{ 0, 76, 0, context->GetSmallBufferAttachment() }
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	UniformLayout squareUniformsLayout(
		{
			//BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t numberOfBuffers, uint32_t bufferAttachment, uint32_t bufferIndex
			{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 1, 0, 1, AccessLevel::ROOT_BUFFER, 1, context->GetUniformAttachment(), 1 } //
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	m_SquareSmallMVP.model = Scale<float>(Eigen::Matrix4f::Identity(), 50.0f, 50.0f, 1.0f);

	TextureLayout squareTextureLayout(
		{
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);



	SamplerLayout squareSamplerLayout(
		{
		}
		);

	StructuredBufferLayout squareStructuredBufferLayout(
		{
		}
	, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	InputInfo squareInputInfoController(squareLayout, squareSmallBufferLayout, squareUniformsLayout, squareTextureLayout, squareSamplerLayout, squareStructuredBufferLayout);

	m_SquareShader.reset(Shader::Instantiate(context, "./assets/shaders/FlatColor", squareInputInfoController));

	m_SquareShader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), squareUniformsLayout.GetElement(1));
	m_SquareVertexBuffer.reset(VertexBuffer::Instantiate(context, (const void*)&squareVertices[0], sizeof(squareVertices), squareLayout.GetStride()));
	m_SquareIndexBuffer.reset(IndexBuffer::Instantiate(context, (const void*)&squareIndices[0], sizeof(squareIndices) / sizeof(uint32_t)));
}

void Lust::ExampleLayer::OnDetach()
{
	m_SquareIndexBuffer.reset();
	m_SquareVertexBuffer.reset();
	m_SquareShader.reset();
	m_Camera.reset();
	m_Texture2.reset();
	m_Texture1.reset();
	m_IndexBuffer.reset();
	m_VertexBuffer.reset();
	m_Shader.reset();
}

void Lust::ExampleLayer::OnUpdate(Timestep ts)
{
	static float maxAxis = 32768.0f;
	static uint8_t squareSmallBuffer[76];
	float rightStickX = (float)Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_RIGHTX);
	float rightStickY = (float)Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_RIGHTY);
	Eigen::Vector2f rightStick(rightStickX, rightStickY);
	if((rightStick.norm() > maxAxis * Input::GetGamepadLowerDeadZone()) && (rightStick.norm() < maxAxis * Input::GetGamepadUpperDeadZone()))
	{
		rightStick.normalize();
		m_CameraRotation += rightStick(0) * m_CameraRotationSpeed * ts;
	}

	if(Input::IsKeyPressed(Key::LUST_KEYCODE_J))
		m_CameraRotation += m_CameraRotationSpeed * ts;
	if (Input::IsKeyPressed(Key::LUST_KEYCODE_L))
		m_CameraRotation -= m_CameraRotationSpeed * ts;

	float leftStickX = (float)Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTX);
	float leftStickY = (float)Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTY);
	Eigen::Vector2f leftStick(leftStickX, leftStickY);
	if ((leftStick.norm() > maxAxis * Input::GetGamepadLowerDeadZone()) && (leftStick.norm() < maxAxis * Input::GetGamepadUpperDeadZone()))
	{
		leftStick.normalize();
		m_CameraPosition.block<2, 1>(0,0) += leftStick * m_CameraTranslationSpeed * ts;
	}

	if (Input::IsKeyPressed(Key::LUST_KEYCODE_A))
		m_CameraPosition(0) -= m_CameraTranslationSpeed * ts;
	if (Input::IsKeyPressed(Key::LUST_KEYCODE_D))
		m_CameraPosition(0) += m_CameraTranslationSpeed * ts;
	if (Input::IsKeyPressed(Key::LUST_KEYCODE_S))
		m_CameraPosition(1) -= m_CameraTranslationSpeed * ts;
	if (Input::IsKeyPressed(Key::LUST_KEYCODE_W))
		m_CameraPosition(1) += m_CameraTranslationSpeed * ts;
	

	//SampleInput();

	m_Camera->SetPosition(m_CameraPosition);
	m_Camera->SetRotation(m_CameraRotation);

	Renderer::BeginScene(*(m_Camera.get()));
	Eigen::Matrix4f squareSmallBufferMatrix;
	for (size_t i = 0; i < 20; i++)
	{
		for (size_t j = 0; j < 20; j++)
		{
			Renderer::SubmitCBV(m_SquareShader, m_SquareShader->GetUniformLayout().GetElement(1));
			Renderer::SubmitShader(m_SquareShader, m_SquareVertexBuffer, m_SquareIndexBuffer);
			squareSmallBufferMatrix = Translate<float>(m_SquareSmallMVP.model, i * 60.0f, j * 60.0f, 0.0f);
			memcpy(&squareSmallBuffer[0], squareSmallBufferMatrix.data(), sizeof(squareSmallBufferMatrix));
			if((i+j)%2 == 0)
				memcpy(&squareSmallBuffer[sizeof(m_SquareSmallMVP.model)], m_SquareColor.data(), sizeof(m_SquareColor));
			else
				memcpy(&squareSmallBuffer[sizeof(m_SquareSmallMVP.model)], m_SquareColor2.data(), sizeof(m_SquareColor2));
			Renderer::SubmitSmallBuffer(m_SquareShader, (void*)&squareSmallBuffer[0], sizeof(squareSmallBuffer), 0);
			RenderCommand::DrawIndexed(m_SquareIndexBuffer->GetCount());
		}
	}
	
	Renderer::EndScene();
	
	
	Renderer::BeginScene(*(m_Camera.get()));
	Renderer::SubmitCBV(m_Shader, m_Shader->GetUniformLayout().GetElement(1));
	Renderer::SubmitCBV(m_Shader, m_Shader->GetUniformLayout().GetElement(2));
	Renderer::SubmitShader(m_Shader, m_VertexBuffer, m_IndexBuffer);
	Renderer::SubmitSmallBuffer(m_Shader, m_SmallMVP.model.data(), sizeof(m_SmallMVP.model), 0);
	RenderCommand::DrawIndexed(m_IndexBuffer->GetCount());
	Renderer::EndScene();
	
}

void Lust::ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Camera", &m_ShowCameraWindow, ImGuiWindowFlags_MenuBar);
	if (ImGui::Button("Reset"))
	{
		m_CameraPosition.setZero();
		m_CameraRotation = 0.0f;
		m_Camera->SetPosition(m_CameraPosition);
		m_Camera->SetRotation(m_CameraRotation);
	}
	ImGui::End();
}

void Lust::ExampleLayer::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizedEvent>(std::bind(&ExampleLayer::OnWindowResize, this, std::placeholders::_1), true);
}

void Lust::ExampleLayer::SampleInput()
{
	if (Input::IsKeyPressed(Key::LUST_KEYCODE_ESCAPE))
		Console::CoreDebug("Escape key pressed");
	if (Input::IsMouseButtonPressed(Mouse::LUST_BUTTON_LEFT))
		Console::CoreDebug("({}, {})", Input::GetMousePosition().first, Input::GetMousePosition().second);
	if (Input::IsGamepadKeyPressed(Gamepad::LUST_GAMEPAD_BUTTON_SOUTH))
		Console::CoreDebug("A/Cross button pressed by player 1");
	if (Input::IsGamepadKeyPressed(Gamepad::LUST_GAMEPAD_BUTTON_SOUTH, 2))
		Console::CoreDebug("A/Cross button pressed by player 2");
	int16_t leftStickX = Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTX);
	int16_t leftStickY = Input::GetGamepadAxis(Gamepad::LUST_GAMEPAD_AXIS_LEFTY);
	if ((leftStickX > 980) && (leftStickY > 980))
		Console::CoreDebug("Left joystick moved ({},{})", leftStickX, leftStickY);
}

bool Lust::ExampleLayer::OnWindowResize(WindowResizedEvent& e)
{
	m_Camera->SetProjection(e.GetWidth() * -.5f, e.GetWidth() *.5f, e.GetHeight() * -.5f, e.GetHeight() * .5f);
	return true;
}


