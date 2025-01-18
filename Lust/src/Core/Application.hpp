#pragma once

#include <memory>
#include "Window.hpp"
#include "ApplicationEvent.hpp"
#include "InputEvent.hpp"

namespace Lust
{
	class LUST_API Application
	{
	public:
		Application();
		~Application();

		void Run();

		void OnEvent(Event& e);

		inline const std::shared_ptr<Window>& GetWindow() const
		{
			return m_Window;
		}

		static void EnableSingleton(Application* ptr);
		static Application* GetInstance();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		bool m_Running = true;

		std::shared_ptr<Window> m_Window;

		static Application* s_AppSingleton;
		static bool s_SingletonEnabled;
	};
}
