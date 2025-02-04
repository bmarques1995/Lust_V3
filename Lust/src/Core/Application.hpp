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

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		static void EnableSingleton(Application* ptr);
		static Application* GetInstance();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		bool m_Running = true;

		std::shared_ptr<Window> m_Window;
		std::unique_ptr<ApplicationStarter> m_Starter;
		std::shared_ptr<GraphicsContext> m_Context;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
		float m_LastCommand = .0f;
		float m_CommandEllapsed = .0f;

		static Application* s_AppSingleton;
		static bool s_SingletonEnabled;
	};
}
