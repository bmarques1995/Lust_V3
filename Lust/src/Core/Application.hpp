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
#include "ImguiWindowController.hpp"
#include "ImguiContext.hpp"
#include "CSOCompiler.hpp"
#include "SPVCompiler.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"

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

		float vBuffer[42] =
		{
			.0f, .5f, .2f, 1.0f, .0f, .0f, 1.0f,
			.5f, -.5f, .2f, .0f, 1.0f, .0f, 1.0f,
			-.5f, -.5f, .2f, .0f, .0f, 1.0f, 1.0f,
			.0f, .4f, .6f, 1.0f, 1.0f, .0f, 1.0f,
			.4f, -.4f, .6f, .0f, 1.0f, 1.0f, 1.0f,
			-.4f, -.4f, .6f, 1.0f, .0f, 1.0f, 1.0f,
		};
		uint32_t iBuffer[6] =
		{
			3,4,5,
			0,1,2,
		};

		bool m_Running = true;

		std::shared_ptr<Window> m_Window;
		std::unique_ptr<ApplicationStarter> m_Starter;
		std::shared_ptr<GraphicsContext> m_Context;
		std::shared_ptr<ImguiWindowController> m_ImguiWindowController;
		std::shared_ptr<ImguiContext> m_ImguiContext;
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<CSOCompiler> m_CSOCompiler;
		std::shared_ptr<SPVCompiler> m_SPVCompiler;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
		float m_LastCommand = .0f;
		float m_CommandEllapsed = .0f;

		static Application* s_AppSingleton;
		static bool s_SingletonEnabled;
	};
}
