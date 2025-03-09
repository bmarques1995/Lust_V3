#include "ExampleLayer.hpp"
#include <imgui/imgui.h>
#include <Renderer.hpp>
#include <Operations.hpp>
#include <Application.hpp>
#include <RenderCommand.hpp>


ExampleLayer::ExampleLayer() :
	Lust::Layer("Example")
{
}

void ExampleLayer::OnAttach()
{
	auto window = Lust::Application::GetInstance()->GetWindow();
	auto context = Lust::Application::GetInstance()->GetContext();

	m_ShaderLibrary.reset(new Lust::ShaderLibrary(context));
	m_Texture2DLibrary.reset(new Lust::Texture2DLibrary(context));

	m_CompleteMVP = {
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity()
	};

	m_CameraController.reset(new Lust::OrthographicCameraController(window->GetWidth(), window->GetHeight(), true));

	m_Texture2DLibrary->Load("./Examples/Layers/assets/textures/yor.png");
	m_Texture2DLibrary->Load("./Examples/Layers/assets/textures/nanao.png");
	m_Texture2DLibrary->Load("./Examples/Layers/assets/textures/sample.png");
	m_Texture2DLibrary->Load("./Examples/Layers/assets/textures/david.jpg");
	m_Texture1 = m_Texture2DLibrary->Get("nanao");
	m_Texture2 = m_Texture2DLibrary->Get("sample");
	m_UniformBuffer.reset(Lust::UniformBuffer::Instantiate(context, &m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP)));
	m_UniformBuffer2.reset(Lust::UniformBuffer::Instantiate(context, &m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP)));

	Eigen::Transform<float, 3, Eigen::Affine> model_transform = Eigen::Translation3f(0.0f, 0.0f, 0.0f) * Eigen::Scaling(m_Texture1->GetWidth() * .5f, m_Texture1->GetHeight() * .5f, 1.0f);

	m_SmallMVP.model = model_transform.matrix();

	m_ShaderReflector.reset(Lust::ShaderReflector::Instantiate("./Examples/Layers/assets/shaders/HelloTriangle", Lust::AllowedStages::VERTEX_STAGE | Lust::AllowedStages::PIXEL_STAGE));
	Lust::InputInfo inputInfoController(m_ShaderReflector->GetInputLayout(), m_ShaderReflector->GetSmallBufferLayout(), m_ShaderReflector->GetUniformLayout(),
		m_ShaderReflector->GetTextureLayout(), m_ShaderReflector->GetSamplerLayout(), m_ShaderReflector->GetStructuredBufferLayout());

	m_ShaderLibrary->Load("./Examples/Layers/assets/shaders/HelloTriangle", inputInfoController);
	m_Shader = m_ShaderLibrary->Get("HelloTriangle");
	Lust::SamplerInfo dynamicSamplerInfo(Lust::SamplerFilter::LINEAR, Lust::AnisotropicFactor::FACTOR_4, Lust::AddressMode::BORDER, Lust::ComparisonPassMode::ALWAYS);
	Lust::SamplerInfo dynamicSamplerInfo2(Lust::SamplerFilter::NEAREST, Lust::AnisotropicFactor::FACTOR_4, Lust::AddressMode::BORDER, Lust::ComparisonPassMode::ALWAYS);
	m_Shader->CreateSampler(m_Shader->GetSamplerLayout().GetElement("dynamicSampler"), dynamicSamplerInfo);
	m_Shader->CreateSampler(m_Shader->GetSamplerLayout().GetElement("dynamicSampler2"), dynamicSamplerInfo2);
	m_Shader->UploadConstantBuffer(&m_UniformBuffer, m_Shader->GetUniformLayout().GetElement("m_CompleteMVP"));
	std::vector<std::shared_ptr<Lust::Texture2D>*> textures;
	textures.push_back(&m_Texture1);
	textures.push_back(&m_Texture2);
	auto textureElements = m_Shader->GetTextureElements();
	size_t i = 0;
	for (auto& textureElement : textureElements)
	{
		m_Shader->UploadTexture2D((textures[i]), textureElement.second);
		i++;
	}
	m_VertexBuffer.reset(Lust::VertexBuffer::Instantiate(context, (const void*)&m_VBuffer[0], sizeof(m_VBuffer), m_Shader->GetInputLayout().GetStride()));
	m_IndexBuffer.reset(Lust::IndexBuffer::Instantiate(context, (const void*)&m_IBuffer[0], sizeof(m_IBuffer) / sizeof(uint32_t)));

	uint32_t rows = 20, cols = 20;
	m_SquareShaderReflector.reset(Lust::ShaderReflector::Instantiate("./Examples/Layers//assets/shaders/FlatColor", Lust::AllowedStages::VERTEX_STAGE | Lust::AllowedStages::PIXEL_STAGE, rows * cols));

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
			size_t offset = (i * rows + j) * sizeof(Eigen::Matrix4f);
			memcpy(m_SSBO + offset, squareSmallBufferMatrix.data(), sizeof(Eigen::Matrix4f));
		}
	}

	m_SquareStructuredBuffer.reset(Lust::StructuredBuffer::Instantiate(context, m_SSBO, rows * cols * sizeof(Eigen::Matrix4f)));

	delete[] m_SSBO;

	Lust::InputInfo squareInputInfoController(m_SquareShaderReflector->GetInputLayout(), m_SquareShaderReflector->GetSmallBufferLayout(), m_SquareShaderReflector->GetUniformLayout(),
		m_SquareShaderReflector->GetTextureLayout(), m_SquareShaderReflector->GetSamplerLayout(), m_SquareShaderReflector->GetStructuredBufferLayout());

	m_ShaderLibrary->Load("./Examples/Layers//assets/shaders/FlatColor", squareInputInfoController);
	m_SquareShader = m_ShaderLibrary->Get("FlatColor");

	m_SquareUniformBuffer.reset(Lust::UniformBuffer::Instantiate(context, &m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP)));

	m_SquareShader->UploadConstantBuffer(&m_SquareUniformBuffer, m_SquareShaderReflector->GetUniformLayout().GetElement("m_CompleteMVP"));
	m_SquareShader->UploadStructuredBuffer(&m_SquareStructuredBuffer, m_SquareShaderReflector->GetStructuredBufferLayout().GetElement("u_InstancedMVP"));
	m_SquareVertexBuffer.reset(Lust::VertexBuffer::Instantiate(context, (const void*)squareVertices, sizeof(squareVertices), m_SquareShaderReflector->GetInputLayout().GetStride()));
	m_SquareIndexBuffer.reset(Lust::IndexBuffer::Instantiate(context, (const void*)squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
}

void ExampleLayer::OnDetach()
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

void ExampleLayer::OnUpdate(Lust::Timestep ts)
{
	static uint8_t squareSmallBuffer[96];

	auto smallBufferIt = m_Shader->GetSmallBufferLayout().GetElement("m_SmallMVP");

	m_CameraController->OnUpdate(ts);
	//SampleInput();

	Lust::Renderer::BeginScene(m_CameraController->GetCamera());
	Eigen::Matrix4f squareSmallBufferMatrix = Eigen::Matrix4f::Identity();

	Lust::Renderer::SubmitCBV(&m_SquareUniformBuffer);
	Lust::Renderer::SubmitShader(m_SquareShader, m_SquareVertexBuffer, m_SquareIndexBuffer);
	memcpy(&squareSmallBuffer[0], squareSmallBufferMatrix.data(), sizeof(squareSmallBufferMatrix));
	memcpy(&squareSmallBuffer[sizeof(m_SquareSmallMVP.model)], m_SquareColor.data(), sizeof(m_SquareColor));
	memcpy(&squareSmallBuffer[sizeof(m_SquareSmallMVP.model) + sizeof(m_SquareColor)], m_SquareColor2.data(), sizeof(m_SquareColor2));
#ifdef LOCATE_CHANGE
#error "Replace with name binding, since the shader is sent"
#endif
	Lust::Renderer::SubmitSmallBuffer(m_SquareShader, (void*)&squareSmallBuffer[0], sizeof(squareSmallBuffer), m_SquareShader->GetSmallBufferLayout().GetElement("m_SmallMVP"));
	Lust::RenderCommand::DrawIndexed(m_SquareIndexBuffer->GetCount(), 400);

	Lust::Renderer::EndScene();


	Lust::Renderer::BeginScene(m_CameraController->GetCamera());
	Lust::Renderer::SubmitCBV(&m_UniformBuffer);
	Lust::Renderer::SubmitCBV(&m_UniformBuffer2);
	Lust::Renderer::SubmitShader(m_Shader, m_VertexBuffer, m_IndexBuffer);
	Lust::Renderer::SubmitSmallBuffer(m_Shader, m_SmallMVP.model.data(), sizeof(m_SmallMVP.model), m_Shader->GetSmallBufferLayout().GetElement("m_SmallMVP"));
	Lust::RenderCommand::DrawIndexed(m_IndexBuffer->GetCount());
	Lust::Renderer::EndScene();

}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Camera", &m_ShowCameraWindow, ImGuiWindowFlags_MenuBar);
	if (ImGui::Button("Reset"))
	{
		m_CameraController->ResetCamera();
	}
	ImGui::End();
}

void ExampleLayer::OnEvent(Lust::Event& event)
{
	m_CameraController->OnEvent(event);
	Lust::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<Lust::WindowResizedEvent>(std::bind(&ExampleLayer::OnWindowResize, this, std::placeholders::_1), true);
}

void ExampleLayer::SampleInput()
{
	if (Lust::Input::IsKeyPressed(Lust::Key::LUST_KEYCODE_ESCAPE))
		Lust::Console::CoreDebug("Escape key pressed");
	if (Lust::Input::IsMouseButtonPressed(Lust::Mouse::LUST_BUTTON_LEFT))
		Lust::Console::CoreDebug("({}, {})", Lust::Input::GetMousePosition().first, Lust::Input::GetMousePosition().second);
	if (Lust::Input::IsGamepadKeyPressed(Lust::Gamepad::LUST_GAMEPAD_BUTTON_SOUTH))
		Lust::Console::CoreDebug("A/Cross button pressed by player 1");
	if (Lust::Input::IsGamepadKeyPressed(Lust::Gamepad::LUST_GAMEPAD_BUTTON_SOUTH, 2))
		Lust::Console::CoreDebug("A/Cross button pressed by player 2");
	int16_t leftStickX = Lust::Input::GetGamepadAxis(Lust::Gamepad::LUST_GAMEPAD_AXIS_LEFTX);
	int16_t leftStickY = Lust::Input::GetGamepadAxis(Lust::Gamepad::LUST_GAMEPAD_AXIS_LEFTY);
	if ((leftStickX > 980) && (leftStickY > 980))
		Lust::Console::CoreDebug("Left joystick moved ({},{})", leftStickX, leftStickY);
}

bool ExampleLayer::OnWindowResize(Lust::WindowResizedEvent& e)
{
	return false;
}