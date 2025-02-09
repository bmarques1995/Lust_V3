#pragma once

#include "LustDLLMacro.hpp"
#include <memory>
#include "Window.hpp"
#include "ApplicationEvent.hpp"
#include "InputEvent.hpp"
#include "Timestep.hpp"
#include "LayerStack.hpp"
#include "ApplicationStarter.hpp"
#include "GraphicsContext.hpp"
#include "GPUInstrumentator.hpp"
#include "CopyPipeline.hpp"

#include "ImguiWindowController.hpp"
#include "ImguiContext.hpp"
#include "CSOCompiler.hpp"
#include "SPVCompiler.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include <Eigen/Eigen>

namespace Lust
{
	class LUST_API Application
	{
	public:
		Application();
		~Application();

		void Run();

		void OnEvent(Event& e);

		inline GraphicsAPI GetCurrentAPI()
		{
			return m_Starter->GetCurrentAPI();;
		}

		inline const std::shared_ptr<Window>& GetWindow() const
		{
			return m_Window;
		}

		inline const std::shared_ptr<GraphicsContext>* GetContext() const
		{
			return (const std::shared_ptr<GraphicsContext>*) (&m_Context);
		}

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		std::shared_ptr<CopyPipeline>* GetCopyPipeline();
		
		static Application* GetInstance();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		static void EnableSingleton(Application* ptr);

		

		std::shared_ptr<CSOCompiler> m_CSOCompiler;
		std::shared_ptr<SPVCompiler> m_SPVCompiler;

		std::shared_ptr<Window> m_Window;
		std::shared_ptr<ImguiWindowController> m_ImguiWindowController;
		std::shared_ptr<GPUInstrumentator> m_Instrumentator;

		std::shared_ptr<GraphicsContext> m_Context;
		std::shared_ptr<ImguiContext> m_ImguiContext;
		std::shared_ptr<CopyPipeline> m_CopyPipeline;
		

		std::unique_ptr<LayerStack> m_LayerStack;
		float m_LastFrameTime = 0.0f;
		float m_LastCommand = .0f;
		float m_CommandEllapsed = .0f;

		static Application* s_AppSingleton;
		static bool s_SingletonEnabled;
		std::unique_ptr<ApplicationStarter> m_Starter;
		bool m_Running = true;
	};
}
