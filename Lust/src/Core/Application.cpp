#include "Application.hpp"
#include <Console.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "CompilerExceptions.hpp"
#include "Input.hpp"
#include "TextureLayout.hpp"
#include "SamplerLayout.hpp"
#include "Operations.hpp"
#include "RenderCommand.hpp"

Lust::Application* Lust::Application::s_AppSingleton = nullptr;
bool Lust::Application::s_SingletonEnabled = false;

Lust::Application::Application()
{
	EnableSingleton(this);

	Console::Init();
	Renderer::Init();
	m_Starter.reset(new ApplicationStarter("controller.json"));
	m_Window.reset(Window::Instantiate());
	m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	m_Window->SetFullScreen(m_Starter->GetFullscreenMode());
	m_Context.reset(GraphicsContext::Instantiate(m_Window.get(), 3));
	RenderCommand::Init(m_Context.get());
	m_CopyPipeline.reset(CopyPipeline::Instantiate(m_Context.get()));

	ImguiContext::StartImgui();

	m_ImguiWindowController.reset(ImguiWindowController::Instantiate(&m_Window));
	m_ImguiContext.reset(ImguiContext::Instantiate(m_Context.get()));

	std::stringstream buffer;
	buffer << m_Window->GetTitle() <<" [" << (m_Starter->GetCurrentAPI() == GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_VK ? "Vulkan" : "D3D12") << "]";
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

	m_Instrumentator.reset(GPUInstrumentator::Instantiate(m_Context.get()));
	m_LayerStack.reset(new LayerStack());
}

Lust::Application::~Application()
{
	m_LayerStack.reset();
	m_Instrumentator.reset();
	m_ImguiContext.reset();
	m_ImguiWindowController.reset();
	ImguiContext::EndImgui();
	m_CopyPipeline.reset();
	RenderCommand::Shutdown();
	m_Context.reset();
	m_Window.reset();
	m_Starter.reset();
	Renderer::Shutdown();
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
					for (Layer* layer : (*m_LayerStack))
						layer->OnUpdate(timestep);

					m_ImguiContext->ReceiveInput();
					ImguiContext::StartFrame();
					{
						for (Layer* layer : (*m_LayerStack))
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

	for (auto it = m_LayerStack->rbegin(); it != m_LayerStack->rend(); ++it)
	{
		(*it)->OnEvent(e);
	}
}

void Lust::Application::PushLayer(Layer* layer)
{
	m_LayerStack->PushLayer(layer);
}

void Lust::Application::PushOverlay(Layer* layer)
{
	m_LayerStack->PushOverlay(layer);
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
