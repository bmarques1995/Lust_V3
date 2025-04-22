#pragma once

#ifdef LUST_USES_WINDOWS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include "Sockets.hpp"

namespace Lust
{
	

	class LUST_API Win32Sockets : public Sockets
	{
		friend class Sockets;
	public:
		void OpenConnection(std::string host, uint16_t port, SocketType socketType, bool isIPv6) override;
		void CloseConnection() override;
		void SendData(const std::string& data) override;
		void Update() override;
	private:
	
		static void StartAPI();
		static void StopAPI();

		
		WSAPOLLFD m_OutDescriptor;
		WSAPOLLFD m_InDescriptor;
		SOCKET m_Socket;
	};
}

#endif