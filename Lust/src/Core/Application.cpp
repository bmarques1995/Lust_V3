#include "Application.hpp"
#include <Console.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "CompilerExceptions.hpp"
#include "Input.hpp"
#include "TextureLayout.hpp"
#include "SamplerLayout.hpp"

Lust::Application* Lust::Application::s_AppSingleton = nullptr;
bool Lust::Application::s_SingletonEnabled = false;

Lust::Application::Application()
{
	EnableSingleton(this);

	m_SmallMVP = {
		Eigen::Matrix4f::Identity()
	};

	m_CompleteMVP = {
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity()
	};

	m_Starter.reset(new ApplicationStarter("controller.json"));
	Console::Init();
	m_Window.reset(Window::Instantiate());
	m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	m_Window->SetFullScreen(m_Starter->GetFullscreenMode());
	m_Context.reset(GraphicsContext::Instantiate(m_Window.get(), 3));
	m_CopyPipeline.reset(CopyPipeline::Instantiate(&m_Context));

	ImguiContext::StartImgui();

	m_ImguiWindowController.reset(ImguiWindowController::Instantiate(&m_Window));
	m_ImguiContext.reset(ImguiContext::Instantiate(&m_Context));

	std::stringstream buffer;
	buffer << "SampleRender Window [" << (m_Starter->GetCurrentAPI() == GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_VK ? "Vulkan" : "D3D12") << "]";
	m_Window->ResetTitle(buffer.str());

	try
	{
		m_SPVCompiler.reset(new SPVCompiler("_main", "_6_8", "1.3"));
		m_CSOCompiler.reset(new CSOCompiler("_main", "_6_8"));
		m_SPVCompiler->PushShaderPath("./assets/shaders/HelloTriangle.hlsl", PipelineType::Graphics);
		m_CSOCompiler->PushShaderPath("./assets/shaders/HelloTriangle.hlsl", PipelineType::Graphics);
		m_SPVCompiler->CompilePackedShader();
		m_CSOCompiler->CompilePackedShader();
	}
	catch (CompilerException e)
	{
		Console::CoreError("{}", e.what());
	}

	InputBufferLayout layout(
		{
			{ShaderDataType::Float3, "POSITION", false},
			{ShaderDataType::Float4, "COLOR", false},
			{ShaderDataType::Float2, "TEXCOORD", false},
		});

	SmallBufferLayout smallBufferLayout(
		{
			//size_t offset, size_t size, uint32_t bindingSlot, uint32_t smallAttachment
			{ 0, 64, 0, m_Context->GetSmallBufferAttachment() }
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	UniformLayout uniformsLayout(
		{
			//BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t numberOfBuffers, uint32_t bufferAttachment
			{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 1, 0, 1, AccessLevel::ROOT_BUFFER, 1, m_Context->GetUniformAttachment() }, //
			{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 2, 0, 2, AccessLevel::ROOT_BUFFER, 1, m_Context->GetUniformAttachment() } //
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	m_Texture1.reset(Texture2D::Instantiate(&m_Context, "./assets/textures/yor.png", 3, 0, 3, 0));
	m_Texture2.reset(Texture2D::Instantiate(&m_Context, "./assets/textures/sample.png", 4, 0, 3, 1));

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

	m_Shader.reset(Shader::Instantiate(&m_Context, "./assets/shaders/HelloTriangle", inputInfoController));

	m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), 1, 1);
	m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), 2, 1);
	m_Shader->UploadTexture2D(&m_Texture1);
	m_Shader->UploadTexture2D(&m_Texture2);
	m_VertexBuffer.reset(VertexBuffer::Instantiate(&m_Context, (const void*)&m_VBuffer[0], sizeof(m_VBuffer), layout.GetStride()));
	m_IndexBuffer.reset(IndexBuffer::Instantiate(&m_Context, (const void*)&iBuffer[0], sizeof(iBuffer) / sizeof(uint32_t)));
	m_Instrumentator.reset(GPUInstrumentator::Instantiate(&m_Context));
}

Lust::Application::~Application()
{
	m_Instrumentator.reset();
	m_Texture2.reset();
	m_Texture1.reset();
	m_IndexBuffer.reset();
	m_VertexBuffer.reset();
	m_Shader.reset();
	m_ImguiContext.reset();
	m_ImguiWindowController.reset();
	ImguiContext::EndImgui();
	m_CopyPipeline.reset();
	m_Context.reset();
	m_Window.reset();
	m_Starter.reset();
	Console::End();
}

void Lust::Application::Run()
{	
	while (m_Running)
	{

		Uint64 now = SDL_GetPerformanceCounter();
		Uint64 frequency = SDL_GetPerformanceFrequency();
		double time = (double)now / (double)frequency;
		Timestep timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;
		m_CommandEllapsed = time;

		if (Input::IsKeyPressed(Key::LUST_KEYCODE_F11) && (m_CommandEllapsed - m_LastCommand) > .2f)
		{
			m_LastCommand = time;
			m_Window->SetFullScreen(!m_Window->IsFullscreen());
			m_Starter->SetFullscreenMode(m_Window->IsFullscreen());
			m_Context->WindowResize(m_Window->GetWidth(), m_Window->GetHeight());
		}

		m_Window->OnUpdate();
		if (!m_Window->IsMinimized())
		{
			try
			{

				m_Context->ReceiveCommands();
				m_Instrumentator->BeginQueryTime();
				m_Context->FillRenderPass();
				m_Context->StageViewportAndScissors();
				m_ImguiWindowController->ReceiveInput();
				{
					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
					//m_Context->Draw(m_IndexBuffer->GetCount());

					m_Shader->Stage();
					m_Shader->BindSmallBuffer(&m_SmallMVP.model(0, 0), sizeof(m_SmallMVP), 0);
					m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), 1, 1);
					m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), 2, 1);
					m_Shader->BindDescriptors();
					m_VertexBuffer->Stage();
					m_IndexBuffer->Stage();
					m_Context->Draw(m_IndexBuffer->GetCount());


					m_ImguiContext->ReceiveInput();
					ImguiContext::StartFrame();
					{
						for (Layer* layer : m_LayerStack)
							layer->OnImGuiRender();

					}
					ImguiContext::EndFrame();
					m_ImguiContext->DispatchInput();


				}
				m_Context->SubmitRenderPass();
				m_Instrumentator->EndQueryTime();
				m_Context->DispatchCommands();
				Console::CoreLog("Ellapsed time: {}", m_Instrumentator->GetQueryTime());
				m_Context->Present();
			}
			catch (GraphicsException e)
			{
				Console::CoreError("Caught error: {}", e.what());
				exit(2);
			}
		}
	}
}

void Lust::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
	dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));
}

void Lust::Application::PushLayer(Layer* layer)
{
	m_LayerStack.PushLayer(layer);
}

void Lust::Application::PushOverlay(Layer* layer)
{
	m_LayerStack.PushOverlay(layer);
}

std::shared_ptr<Lust::CopyPipeline>* Lust::Application::GetCopyPipeline()
{
	return &m_CopyPipeline;
}

bool Lust::Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}

bool Lust::Application::OnWindowResize(WindowResizeEvent& e)
{
	m_Context->WindowResize(e.GetWidth(), e.GetHeight());
	return true;
}

void Lust::Application::EnableSingleton(Application* ptr)
{
	if (!s_SingletonEnabled)
	{
		s_SingletonEnabled = true;
		s_AppSingleton = ptr;
	}
}

Lust::Application* Lust::Application::GetInstance()
{
	return s_AppSingleton;
}
