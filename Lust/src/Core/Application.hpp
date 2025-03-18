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
	/**
	* @brief %Application class
	* @details application is the first class to be initialized, is the controller of the whole application
	*/
	class LUST_API Application
	{
	public:
		/**
		* @brief Application constructor
		*/
		Application();
		/**
		* @brief Application destructor
		*/
		~Application();

		/**
		* @brief Run the application, main loop
		*  
		*/
		void Run();

		/**
		* @brief OnEvent callback
		* @details resolve registered events
		* @param e Event
		*/
		void OnEvent(Event& e);

		/**
		* @brief Gets the current Graphics API (Vulkan/D3D12)
		*/
		inline GraphicsAPI GetCurrentAPI()
		{
			return m_Starter->GetCurrentAPI();;
		}

		/**
		* @brief Gets the window
		*/
		inline const Window* GetWindow() const
		{
			return m_Window.get();
		}

		/**
		* @brief Gets the context
		*/
		inline const GraphicsContext* GetContext() const
		{
			return m_Context.get();
		}

		/**
		* @brief Gets the full acessible context, where you can modify the context, e.g. change the clear color
		*/
		inline GraphicsContext* GetEditableContext()
		{
			return m_Context.get();
		}

		/**
		* @brief Upload a layer to the application
		*/
		void PushLayer(Layer* layer);
		/**
		* @brief Upload a layer to the last element of the list
		*/
		void PushOverlay(Layer* layer);

		/**
		* @brief Gets the copy pipeline
		*/
		std::shared_ptr<CopyPipeline>* GetCopyPipeline();
		
		/**
		* @brief Gets the current gpu timestep
		*/
		double GetGPUTime() const { return m_GPUTime; }
		/**
		* @brief Gets the singleton of the application
		*/
		static Application* GetInstance();

	protected:
		/**
		* @brief Render action
		* @details render the scene, is used in the main loop as the render loop
		*/
		void RenderAction();
		/**
		* @brief Handles window close event
		*/
		bool OnWindowClose(WindowClosedEvent& e);
		/**
		* @brief Handles window resize event
		*/
		bool OnWindowResize(WindowResizedEvent& e);
		/**
		* @brief Handles key down event
		*/
		bool OnGamepadKeydown(GamepadKeyPressedEvent& e);
		/**
		* @brief Handles key up event
		*/
		bool OnGamepadAxisMotion(GamepadAxisMovedEvent& e);
		/**
		* @brief Handles mouse move event
		*/
		bool OnMouseMove(MouseMovedEvent& e);
		/**
		* @brief Handles mouse button down event
		*/
		bool OnMouseButtonPress(MouseButtonPressedEvent& e);
		/**
		* @brief Handles mouse wheel event
		*/
		bool OnMouseWheel(MouseScrolledEvent& e);

		/**
		* @brief Enables the singleton of the application
		*/
		static void EnableSingleton(Application* ptr);

		std::shared_ptr<CSOCompiler> m_CSOCompiler;
		std::shared_ptr<SPVCompiler> m_SPVCompiler;

		std::unique_ptr<Window> m_Window;
		std::unique_ptr<ImguiWindowController> m_ImguiWindowController;
		std::shared_ptr<GPUInstrumentator> m_Instrumentator;

		std::unique_ptr<GraphicsContext> m_Context;
		std::unique_ptr<ImguiContext> m_ImguiContext;
		std::shared_ptr<CopyPipeline> m_CopyPipeline;

		std::unique_ptr<LayerStack> m_LayerStack;
		float m_LastFrameTime = 0.0f;
		float m_LastCommand = .0f;
		float m_CommandEllapsed = .0f;
		double m_GPUTime = 0.0f;

		static Application* s_AppSingleton;
		static bool s_SingletonEnabled;
		std::unique_ptr<ApplicationStarter> m_Starter;
		bool m_Running = true;
	};
}
