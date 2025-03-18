#pragma once

#include "LustDLLMacro.hpp"
#include "Timestep.hpp"
#include "Event.hpp"
#include <sstream>

namespace Lust
{
	/**
	* @brief %Layer class, the base class of your logic
	* @details Layer class is a key component of the application, here you can add your own logic.
	* The application will run a set of layers, that can be defined outside of the application. Using a subclass of application
	* you can create your own layers and add them to the application.
	*/
	class LUST_API Layer
	{
	public:
		/**
		* @brief Layer constructor
		* @param name name of the layer
		*/
		Layer(const std::string& name = "Layer");
		/**
		* @brief Layer destructor
		*/
		virtual ~Layer() = default;

		/**
		* @brief Called when the layer is attached to the application, in another words, the constructor of the layer
		*/
		virtual void OnAttach() {}
		/**
		* @brief Called when the layer is detached from the application, in another words, the destructor of the layer
		*/
		virtual void OnDetach() {}
		/**
		* @brief Called when the layer is updated, the loop logic is here
		*/
		virtual void OnUpdate(Timestep ts) {}
		/**
		* @brief Called to update ImGui, will be used only for fast debug, since the editor will be based on Qt
		*/
		virtual void OnImGuiRender() {}
		/**
		* @brief Called when an event is triggered
		*/
		virtual void OnEvent(Event& event) {}

		/**
		* @brief Gets the name of the layer
		*/
		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}
