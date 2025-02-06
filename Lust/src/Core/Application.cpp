#include "Application.hpp"
#include <Console.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "CompilerExceptions.hpp"
#include "Input.hpp"

Lust::Application* Lust::Application::s_AppSingleton = nullptr;
bool Lust::Application::s_SingletonEnabled = false;

Lust::Application::Application()
{
	EnableSingleton(this);
	m_Starter.reset(new ApplicationStarter("controller.json"));
	Console::Init();
	m_Window.reset(Window::Instantiate());
	m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	m_Window->SetFullScreen(m_Starter->GetFullscreenMode());
	m_Context.reset(GraphicsContext::Instantiate(m_Window.get(), 3));

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
		});

	m_Shader.reset(Shader::Instantiate(&m_Context, "./assets/shaders/HelloTriangle", layout));

	//m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), 1, 1);
	//m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), 1, 2);
	m_VertexBuffer.reset(VertexBuffer::Instantiate(&m_Context, (const void*)&vBuffer[0], sizeof(vBuffer), layout.GetStride()));
	m_IndexBuffer.reset(IndexBuffer::Instantiate(&m_Context, (const void*)&iBuffer[0], sizeof(iBuffer) / sizeof(uint32_t)));
}

Lust::Application::~Application()
{
	m_IndexBuffer.reset();
	m_VertexBuffer.reset();
	m_Shader.reset();
	m_ImguiContext.reset();
	m_ImguiWindowController.reset();
	ImguiContext::EndImgui();
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
		m_Window->OnUpdate();
		m_ImguiWindowController->ReceiveInput();

		if (Input::IsKeyPressed(Key::LUST_KEYCODE_F1) && (m_CommandEllapsed - m_LastCommand) > .2f)
		{
			m_LastCommand = time;
			m_Window->SetFullScreen(!m_Window->IsFullscreen());
			m_Starter->SetFullscreenMode(m_Window->IsFullscreen());
			m_Context->WindowResize(m_Window->GetWidth(), m_Window->GetHeight());
		}

		m_Context->ReceiveCommands();
		m_Context->StageViewportAndScissors();
		
		m_Shader->Stage();
		m_VertexBuffer->Stage();
		m_IndexBuffer->Stage();
		m_Context->Draw(m_IndexBuffer->GetCount());
		
		for (Layer* layer : m_LayerStack)
			layer->OnUpdate(timestep);

		m_ImguiContext->ReceiveInput();
		ImguiContext::StartFrame();
		{
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();

		}
		ImguiContext::EndFrame();
		m_ImguiContext->DispatchInput();

		m_Context->DispatchCommands();
		m_Context->Present();
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
