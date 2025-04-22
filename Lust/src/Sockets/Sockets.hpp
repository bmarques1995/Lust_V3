#pragma once

#include "LustDLLMacro.hpp"
#include <string>
#include <cstdint>
#include <functional>
#include "InputEvent.hpp"
#include "SocketDefs.hpp"

namespace Lust
{
	using EventCallbackFn = std::function<void(Event&)>;

	class LUST_API Sockets
	{
	public:

		static void StartAPI();
		static void StopAPI();

		virtual void OpenConnection(std::string host, uint16_t port, SocketType socketType, bool isIPv6) = 0;
		virtual void CloseConnection() = 0;
		virtual void SendData(const std::string& data) = 0;
		virtual void Update() = 0;

		static void SetCallback(std::function<void(Event&)> callback);

		static Sockets* Instantiate();
	
	protected:

		std::string m_Host;
		uint16_t m_Port;
		SocketType m_SocketType;
		bool m_IsIPv6 = false;

		static EventCallbackFn s_Callback;
	};
}