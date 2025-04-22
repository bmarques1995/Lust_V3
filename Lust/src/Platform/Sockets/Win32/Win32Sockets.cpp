#include "Win32Sockets.hpp"
#include "Console.hpp"

void Lust::Win32Sockets::OpenConnection(std::string host, uint16_t port, SocketType socketType, bool isIPv6)
{
	m_Host = host;
	m_Port = port;
	m_SocketType = socketType;
	m_IsIPv6 = isIPv6;

	static const std::unordered_map<SocketType, int32_t> socketTypeMap = {
		{ SocketType::TCP, SOCK_STREAM },
		{ SocketType::UDP, SOCK_DGRAM }
	};

	static const std::unordered_map<SocketType, int32_t> protocolMap = {
		{ SocketType::TCP, IPPROTO_TCP },
		{ SocketType::UDP, IPPROTO_UDP }
	};

	int32_t nativeProtocol = IPPROTO_TCP;
	int32_t socketTypeNative = SOCK_STREAM;
	auto it = socketTypeMap.find(socketType);
	if (it != socketTypeMap.end())
		socketTypeNative = it->second;
	auto it2 = protocolMap.find(socketType);
	if (it2 != protocolMap.end())
		nativeProtocol = it2->second;

    m_Socket = socket(isIPv6 ? AF_INET6 : AF_INET, socketTypeNative, nativeProtocol);
    if (m_Socket == INVALID_SOCKET)
    {
        Console::CoreCritical("socket() failed");
    }

    u_long mode = 1;
    ioctlsocket(m_Socket, FIONBIO, &mode); // Set non-blocking

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_Port);
    inet_pton(AF_INET, m_Host.c_str(), &server_addr.sin_addr);

    int result = connect(m_Socket, (sockaddr*)&server_addr, sizeof(server_addr));
    if (result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
        Console::CoreCritical("connect() failed: {}", WSAGetLastError());
    }

    m_InDescriptor.events = POLLIN;
    m_InDescriptor.fd = m_Socket;
    m_OutDescriptor.events = POLLWRNORM;
	m_OutDescriptor.fd = m_Socket;

    result = WSAPoll(&m_InDescriptor, 1, 60);
    if (result <= 0 || !(m_OutDescriptor.revents & POLLWRNORM)) {
        Console::CoreCritical("[Client] Connection timeout or failure");
    }

    // Check for errors
    int so_error = 0;
    int len = sizeof(so_error);
    getsockopt(m_Socket, SOL_SOCKET, SO_ERROR, (char*)&so_error, &len);
    if (so_error != 0) {
        Console::CoreCritical("[Client] SO_ERROR: {}", so_error);
    }

	
    SocketConnectedEvent e(m_Host, m_Port, m_SocketType, m_IsIPv6);
	s_Callback(e);
}

void Lust::Win32Sockets::CloseConnection()
{
    SocketDisconnectedEvent e(m_Host, m_Port, m_SocketType, m_IsIPv6);
    s_Callback(e);
    shutdown(m_Socket, SD_BOTH);
    closesocket(m_Socket);
}

void Lust::Win32Sockets::SendData(const std::string& data)
{
    m_OutDescriptor.events = POLLWRNORM;
    if (send(m_Socket, data.c_str(), (data.size() + 1), 0) == SOCKET_ERROR)
    {
        Console::CoreError("send() failed");
    }
    else
    {
        auto result = WSAPoll(&m_OutDescriptor, 1, 10);
        Console::CoreDebug("[Client] Message sent.");
        SocketDataSentEvent e(m_Host, m_Port, m_SocketType, m_IsIPv6, data);
        s_Callback(e);
    }
}

void Lust::Win32Sockets::Update()
{
    int poll_result = WSAPoll(&m_InDescriptor, 1, 10); // Wait up to 5 seconds
    m_InDescriptor.events = POLLIN;
    if (poll_result > 0 && (m_InDescriptor.revents & POLLIN))
    {
        char buf[1024];
        int received = recv(m_Socket, buf, sizeof(buf) - 1, 0);
        if (received > 0)
        {
            buf[received] = '\0';
            SocketDataReceivedEvent e(m_Host, m_Port, m_SocketType, m_IsIPv6, buf);
            s_Callback(e);
            Console::CoreDebug("[Client] Received: {}", buf);
        }
        else if (received == 0)
        {
            Console::CoreError("[Client] Server closed connection.");
        }
        else
        {
            Console::CoreError("recv() failed: {}", WSAGetLastError());
        }
    }
}

void Lust::Win32Sockets::StartAPI()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		Console::CoreCritical("Failed to initialize Winsock");
	}
}

void Lust::Win32Sockets::StopAPI()
{
	WSACleanup();
}
