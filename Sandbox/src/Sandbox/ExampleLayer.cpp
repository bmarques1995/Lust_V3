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
	
	m_ShaderLibrary.reset(new ShaderLibrary(context));
	m_Texture2DLibrary.reset(new Texture2DLibrary(context));

	m_CompleteMVP = {
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity()
	};

	m_CameraController.reset(new OrthographicCameraController(window->GetWidth(), window->GetHeight(), true));

	m_Texture2DLibrary->Load("./assets/textures/yor.png");
	m_Texture2DLibrary->Load("./assets/textures/nanao.png");
	m_Texture2DLibrary->Load("./assets/textures/sample.png");
	m_Texture2DLibrary->Load("./assets/textures/david.jpg");
	m_Texture1 = m_Texture2DLibrary->Get("nanao");
	m_Texture2 = m_Texture2DLibrary->Get("sample");
	m_UniformBuffer.reset(UniformBuffer::Instantiate(context, &m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP)));
	m_UniformBuffer2.reset(UniformBuffer::Instantiate(context, &m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP)));

	Eigen::Transform<float, 3, Eigen::Affine> model_transform = Eigen::Translation3f(0.0f, 0.0f, 0.0f) * Eigen::Scaling(m_Texture1->GetWidth() * .5f, m_Texture1->GetHeight() * .5f, 1.0f);

	m_SmallMVP.model = model_transform.matrix();
	
	m_ShaderReflector.reset(ShaderReflector::Instantiate("./assets/shaders/HelloTriangle", AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE));
	InputInfo inputInfoController(m_ShaderReflector->GetInputLayout(), m_ShaderReflector->GetSmallBufferLayout(), m_ShaderReflector->GetUniformLayout(), 
		m_ShaderReflector->GetTextureLayout(), m_ShaderReflector->GetSamplerLayout(), m_ShaderReflector->GetStructuredBufferLayout());

	m_ShaderLibrary->Load("./assets/shaders/HelloTriangle", inputInfoController);
	m_Shader = m_ShaderLibrary->Get("HelloTriangle");
	SamplerInfo dynamicSamplerInfo(SamplerFilter::LINEAR, AnisotropicFactor::FACTOR_4, AddressMode::BORDER, ComparisonPassMode::ALWAYS);
	SamplerInfo dynamicSamplerInfo2(SamplerFilter::NEAREST, AnisotropicFactor::FACTOR_4, AddressMode::BORDER, ComparisonPassMode::ALWAYS);
	m_Shader->CreateSampler(m_Shader->GetSamplerLayout().GetElement("dynamicSampler"), dynamicSamplerInfo);
	m_Shader->CreateSampler(m_Shader->GetSamplerLayout().GetElement("dynamicSampler2"), dynamicSamplerInfo2);
	m_Shader->UploadConstantBuffer(&m_UniformBuffer, m_Shader->GetUniformLayout().GetElement("m_CompleteMVP"));
	std::vector<std::shared_ptr<Texture2D>*> textures;
	textures.push_back(&m_Texture1);
	textures.push_back(&m_Texture2);
	auto textureElements = m_Shader->GetTextureElements();
	size_t i = 0;
	for (auto& textureElement : textureElements)
	{
		m_Shader->UploadTexture2D((textures[i]), textureElement.second);
		i++;
	}
	m_VertexBuffer.reset(VertexBuffer::Instantiate(context, (const void*)&m_VBuffer[0], sizeof(m_VBuffer), m_Shader->GetInputLayout().GetStride()));
	m_IndexBuffer.reset(IndexBuffer::Instantiate(context, (const void*)&m_IBuffer[0], sizeof(m_IBuffer) / sizeof(uint32_t)));
	
	uint32_t rows = 20, cols = 20;
	m_SquareShaderReflector.reset(ShaderReflector::Instantiate("./assets/shaders/FlatColor", AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE, rows * cols));
	
	m_SSBO = new uint8_t[m_SquareShaderReflector->GetStructuredBufferLayout().GetElement("u_InstancedMVP").GetSize()];

	Eigen::Matrix4f squareSmallBufferMatrix;
	Eigen::AlignedScaling3f baseScale = Eigen::Scaling(50.0f, 50.0f, 1.0f);
	Eigen::Transform<float, 3, Eigen::Affine> element_transform;
	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
		{
			element_transform = Eigen::Translation<float, 3>(i * 60.0f, j * 60.0f, 0.0f) * baseScale;
			squareSmallBufferMatrix = element_transform.matrix().transpose();
			size_t offset = (i * rows + j)*sizeof(Eigen::Matrix4f);
			memcpy(m_SSBO + offset, squareSmallBufferMatrix.data(), sizeof(Eigen::Matrix4f));
		}
	}

	m_SquareStructuredBuffer.reset(StructuredBuffer::Instantiate(context, m_SSBO, rows * cols * sizeof(Eigen::Matrix4f)));

	delete[] m_SSBO;
	
	InputInfo squareInputInfoController(m_SquareShaderReflector->GetInputLayout(), m_SquareShaderReflector->GetSmallBufferLayout(), m_SquareShaderReflector->GetUniformLayout(),
		m_SquareShaderReflector->GetTextureLayout(), m_SquareShaderReflector->GetSamplerLayout(), m_SquareShaderReflector->GetStructuredBufferLayout());

	m_ShaderLibrary->Load("./assets/shaders/FlatColor", squareInputInfoController);
	m_SquareShader = m_ShaderLibrary->Get("FlatColor");

	m_SquareUniformBuffer.reset(UniformBuffer::Instantiate(context, &m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP)));

	m_SquareShader->UploadConstantBuffer(&m_SquareUniformBuffer, m_SquareShaderReflector->GetUniformLayout().GetElement("m_CompleteMVP"));
	m_SquareShader->UploadStructuredBuffer(&m_SquareStructuredBuffer, m_SquareShaderReflector->GetStructuredBufferLayout().GetElement("u_InstancedMVP"));
	m_SquareVertexBuffer.reset(VertexBuffer::Instantiate(context, (const void*)squareVertices, sizeof(squareVertices), m_SquareShaderReflector->GetInputLayout().GetStride()));
	m_SquareIndexBuffer.reset(IndexBuffer::Instantiate(context, (const void*)squareIndices, sizeof(squareIndices)/ sizeof(uint32_t)));
}

void Lust::ExampleLayer::OnDetach()
{
	m_SquareIndexBuffer.reset();
	m_SquareVertexBuffer.reset();
	m_SquareShader.reset();
	m_CameraController.reset();
	m_Texture2.reset();
	m_Texture1.reset();
	m_IndexBuffer.reset();
	m_VertexBuffer.reset();
	m_Shader.reset();
	m_Texture2DLibrary.reset();
	m_ShaderLibrary.reset();
}

void Lust::ExampleLayer::OnUpdate(Timestep ts)
{
	static uint8_t squareSmallBuffer[96];
	
	auto smallBufferIt = m_Shader->GetSmallBufferLayout().GetElement("m_SmallMVP");

	m_CameraController->OnUpdate(ts);
	//SampleInput();

	Renderer::BeginScene(m_CameraController->GetCamera());
	Eigen::Matrix4f squareSmallBufferMatrix = Eigen::Matrix4f::Identity();

	Renderer::SubmitCBV(&m_SquareUniformBuffer);
	Renderer::SubmitShader(m_SquareShader, m_SquareVertexBuffer, m_SquareIndexBuffer);
	memcpy(&squareSmallBuffer[0], squareSmallBufferMatrix.data(), sizeof(squareSmallBufferMatrix));
	memcpy(&squareSmallBuffer[sizeof(m_SquareSmallMVP.model)], m_SquareColor.data(), sizeof(m_SquareColor));
	memcpy(&squareSmallBuffer[sizeof(m_SquareSmallMVP.model) + sizeof(m_SquareColor)], m_SquareColor2.data(), sizeof(m_SquareColor2));
#ifdef LOCATE_CHANGE
#error "Replace with name binding, since the shader is sent"
#endif
	Renderer::SubmitSmallBuffer(m_SquareShader, (void*)&squareSmallBuffer[0], sizeof(squareSmallBuffer), m_SquareShader->GetSmallBufferLayout().GetElement("m_SmallMVP"));
	RenderCommand::DrawIndexed(m_SquareIndexBuffer->GetCount(), 400);

	Renderer::EndScene();
	
	
	Renderer::BeginScene(m_CameraController->GetCamera());
	Renderer::SubmitCBV(&m_UniformBuffer);
	Renderer::SubmitCBV(&m_UniformBuffer2);
	Renderer::SubmitShader(m_Shader, m_VertexBuffer, m_IndexBuffer);
	Renderer::SubmitSmallBuffer(m_Shader, m_SmallMVP.model.data(), sizeof(m_SmallMVP.model), m_Shader->GetSmallBufferLayout().GetElement("m_SmallMVP"));
	RenderCommand::DrawIndexed(m_IndexBuffer->GetCount());
	Renderer::EndScene();
	
}

void Lust::ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Camera", &m_ShowCameraWindow, ImGuiWindowFlags_MenuBar);
	if (ImGui::Button("Reset"))
	{
		m_CameraController->ResetCamera();
	}
	ImGui::End();
}

void Lust::ExampleLayer::OnEvent(Event& event)
{
	m_CameraController->OnEvent(event);
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
	return false;
}


