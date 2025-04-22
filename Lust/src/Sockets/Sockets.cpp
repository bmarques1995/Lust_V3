#include "Sockets.hpp"

#ifdef LUST_USES_WINDOWS
#include "Win32Sockets.hpp"
#endif

using EventCallbackFn = std::function<void(Lust::Event&)>;
EventCallbackFn Lust::Sockets::s_Callback = nullptr;

void Lust::Sockets::StartAPI()
{
#ifdef LUST_USES_WINDOWS
	Win32Sockets::StartAPI();
#endif
}

void Lust::Sockets::StopAPI()
{
#ifdef LUST_USES_WINDOWS
	Win32Sockets::StopAPI();
#endif
}

void Lust::Sockets::SetCallback(EventCallbackFn callback)
{
	s_Callback = callback;
}

Lust::Sockets* Lust::Sockets::Instantiate()
{
#ifdef LUST_USES_WINDOWS
	return new Win32Sockets();
#endif
}