#include "Application.hpp"
#include <Console.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "CompilerExceptions.hpp"
#include "Input.hpp"
#include "TextureLayout.hpp"
#include "Instrumentator.hpp"
#include "SamplerLayout.hpp"
#include "Operations.hpp"
#include "RenderCommand.hpp"
#include "Renderer2D.hpp"
#include "Sleeper.hpp"
#include "Sockets.hpp"
#include <ShaderReflector.hpp>

Lust::Application* Lust::Application::s_AppSingleton = nullptr;
bool Lust::Application::s_SingletonEnabled = false;

Lust::Application::Application()
{
	EnableSingleton(this);

	Sleeper::SetFrameTime(1.0 / 60.0);
	Sleeper::SetFrameTimeF(1.0f / 60.0f);
	Renderer::Init();
	m_Starter.reset(new ApplicationStarter("controller.json"));
	WindowProps props("Lust Engine", m_Starter->GetWidth(), m_Starter->GetHeight());
	m_Window.reset(Window::Instantiate(props));
	m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	m_Window->SetFullScreen(m_Starter->GetFullscreenMode());
	m_Context.reset(GraphicsContext::Instantiate(m_Window.get(), 3));
	Sockets::StartAPI();
	Sockets::SetCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	ShaderReflector::InitAPIInfos(m_Context->GetUniformAttachment(), m_Context->GetSmallBufferAttachment(), CastLustToShaderManagerAPI(GetCurrentAPI()));

	try
	{
		m_SPVCompiler.reset(new SPVCompiler("_main", "_6_8", "1.3"));
		m_CSOCompiler.reset(new CSOCompiler("_main", "_6_8"));
		m_SPVCompiler->PushShaderPath("./assets/shaders/Renderer2D.hlsl", PipelineType::Graphics);
		m_CSOCompiler->PushShaderPath("./assets/shaders/Renderer2D.hlsl", PipelineType::Graphics);
		m_SPVCompiler->CompilePackedShader();
		m_CSOCompiler->CompilePackedShader();
	}
	catch (CompilerException e)
	{
		Console::CoreError("{}", e.what());
	}

	TextureLibrary::InitCopyPipeline(m_Context.get());

	Console::CoreLog("Current GPU: {}", m_Context->GetGPUName().c_str());
	RenderCommand::Init(m_Context.get());
	Renderer2D::Instantiate();
	m_CopyPipeline.reset(CopyPipeline::Instantiate(m_Context.get()));

	ImguiContext::StartImgui();

	m_ImguiWindowController.reset(ImguiWindowController::Instantiate(m_Window.get()));
	m_ImguiContext.reset(ImguiContext::Instantiate(m_Context.get()));

	std::stringstream buffer;
	buffer << m_Window->GetTitle() <<" [" << (m_Starter->GetCurrentAPI() == GraphicsAPI::RENDER_GRAPHICS_API_VK ? "Vulkan" : "D3D12") << "]";
	m_Window->ResetTitle(buffer.str());

	m_Instrumentator.reset(GPUInstrumentator::Instantiate(m_Context.get()));
	m_LayerStack.reset(new LayerStack());

	RenderAction();
	m_Window->DisplayWindow();
}

Lust::Application::~Application()
{
	if(!m_Destroyed)
		DestroyApplication();
}

void Lust::Application::Run()
{	
	while (m_Running)
	{
		GameLoop();
	}
}

void Lust::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowClosedEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1), false);
	dispatcher.Dispatch<WindowResizedEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1), true);
	dispatcher.Dispatch<GamepadKeyPressedEvent>(std::bind(&Application::OnGamepadKeydown, this, std::placeholders::_1), false);
	dispatcher.Dispatch<GamepadAxisMovedEvent>(std::bind(&Application::OnGamepadAxisMotion, this, std::placeholders::_1), false);
	dispatcher.Dispatch<MouseMovedEvent>(std::bind(&Application::OnMouseMove, this, std::placeholders::_1), false);
	dispatcher.Dispatch<MouseButtonPressedEvent>(std::bind(&Application::OnMouseButtonPress, this, std::placeholders::_1), false);
	dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&Application::OnMouseWheel, this, std::placeholders::_1), false);
	dispatcher.Dispatch<SocketConnectedEvent>(std::bind(&Application::OnSocketConnect, this, std::placeholders::_1), false);
	dispatcher.Dispatch<SocketDisconnectedEvent>(std::bind(&Application::OnSocketDisconnect, this, std::placeholders::_1), false);
	dispatcher.Dispatch<SocketDataSentEvent>(std::bind(&Application::OnSocketSend, this, std::placeholders::_1), false);
	dispatcher.Dispatch<SocketDataReceivedEvent>(std::bind(&Application::OnSocketReceive, this, std::placeholders::_1), false);

	if(m_LayerStack)
		for (auto it = m_LayerStack->rbegin(); it != m_LayerStack->rend(); ++it)
		{
			if (e.IsHandled())
				break;
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

bool Lust::Application::ProceedClose()
{
	return true;
}

std::shared_ptr<Lust::CopyPipeline>* Lust::Application::GetCopyPipeline()
{
	return &m_CopyPipeline;
}

void Lust::Application::GameLoop()
{
	Lust::InstrumentationTimer timer("Frametime");
	RenderAction();
	timer.Stop();
}

void Lust::Application::DestroyApplication()
{
	for (Layer* layer : *m_LayerStack)
		layer->OnDetach();
	m_LayerStack.reset();
	m_Instrumentator.reset();
	m_ImguiContext.reset();
	m_ImguiWindowController.reset();
	ImguiContext::EndImgui();
	m_CopyPipeline.reset();
	Renderer2D::Destroy();
	RenderCommand::Shutdown();
	TextureLibrary::DestroyCopyPipeline();
	m_Context.reset();
	m_Window.reset();
	m_Starter.reset();
	Renderer::Shutdown();
	
	Sockets::StopAPI();
	m_Destroyed = true;
}

void Lust::Application::RenderAction()
{
	Sleeper::StartTracking();
	Timestep timestep = Sleeper::GetDeltaTime();
	m_LastFrameTime = Sleeper::GetCurrentFrameTime();
	m_CommandEllapsed = Sleeper::GetCurrentFrameTime();
	double commandTimeEllapsed = (std::chrono::duration_cast<std::chrono::microseconds>(m_CommandEllapsed - m_LastCommand).count() / 1e6);

	if (Input::IsKeyPressed(Key::LUST_KEYCODE_F11) && commandTimeEllapsed > .2f)
	{
		m_LastCommand = Sleeper::GetCurrentFrameTime();
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
			m_GPUTime = m_Instrumentator->GetQueryTime();
			m_Context->Present();
		}
		catch (GraphicsException e)
		{
			Console::CoreError("Caught error: {}", e.what());
			exit(2);
		}
	}
	Sleeper::Sleep();
}

bool Lust::Application::OnWindowClose(WindowClosedEvent& e)
{
	bool proceed = ProceedClose();
	if (ProceedClose())
	{
		m_Window->EmitClose();
		m_Running = false;
	}
	return true;
}

bool Lust::Application::OnWindowResize(WindowResizedEvent& e)
{
	m_Context->WindowResize(e.GetWidth(), e.GetHeight());
	return true;
}

bool Lust::Application::OnGamepadKeydown(GamepadKeyPressedEvent& e)
{
	m_Window->DisplayCursor(false);
	return true;
}

bool Lust::Application::OnGamepadAxisMotion(GamepadAxisMovedEvent& e)
{
	m_Window->DisplayCursor(false);
	return true;
}

bool Lust::Application::OnMouseMove(MouseMovedEvent& e)
{
	m_Window->DisplayCursor(true);
	return true;
}

bool Lust::Application::OnMouseButtonPress(MouseButtonPressedEvent& e)
{
	m_Window->DisplayCursor(true);
	return true;
}

bool Lust::Application::OnMouseWheel(MouseScrolledEvent& e)
{
	m_Window->DisplayCursor(true);
	return false;
}

bool Lust::Application::OnSocketConnect(SocketConnectedEvent& e)
{
	return true;
}

bool Lust::Application::OnSocketDisconnect(SocketDisconnectedEvent& e)
{
	return true;
}

bool Lust::Application::OnSocketSend(SocketDataSentEvent& e)
{
	return true;
}

bool Lust::Application::OnSocketReceive(SocketDataReceivedEvent& e)
{
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

Lust::TargetAPI Lust::Application::CastLustToShaderManagerAPI(GraphicsAPI api)
{
	switch (api)
	{
	default:
	case Lust::RENDER_GRAPHICS_API_VK:
		return TargetAPI::SHADER_MANAGER_GRAPHICS_API_VK;
	case Lust::RENDER_GRAPHICS_API_D3D12:
		return TargetAPI::SHADER_MANAGER_GRAPHICS_API_D3D12;
	}
}

