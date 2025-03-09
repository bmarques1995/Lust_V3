#ifdef LUST_USES_WINDOWS

#include <d3d12.h>

extern "C"
{
	__declspec(dllexport) extern const UINT D3D12SDKVersion = 615;
	__declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\";
}

#endif