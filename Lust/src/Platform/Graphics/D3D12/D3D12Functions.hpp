#pragma once

#ifdef LUST_USES_WINDOWS

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <D3D12MemAlloc.h>

typedef HRESULT(WINAPI* PFN_CreateDXGIFactory2)(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory);
typedef HRESULT(WINAPI* PFN_DXGIGetDebugInterface1)(UINT Flags, REFIID riid, _COM_Outptr_ void** pDebug);

typedef HRESULT(WINAPI* PFN_D3D12CreateDevice)(IUnknown* pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, REFIID riid, void** ppDevice);
typedef HRESULT(WINAPI* PFN_D3D12GetDebugInterface)(REFIID riid, _COM_Outptr_ void** ppvObject);

typedef HRESULT(WINAPI* PFN_D3D12MACreateAllocator)(const D3D12MA::ALLOCATOR_DESC* pDesc, D3D12MA::Allocator** ppAllocator);

namespace Lust
{
	class D3D12Functions
	{
	public:
		static void LoadD3D12Functions();
		static void UnloadD3D12Functions();

		static void LoadMockD3D12Functions();

		static bool IsLoaded();

		//HRESULT WINAPI CreateDXGIFactory2(UINT Flags, REFIID riid, _COM_Outptr_ void **ppFactory)
		static bool s_IsLoaded;
		static PFN_D3D12CreateDevice D3D12CreateDeviceFn;
		static PFN_D3D12GetDebugInterface D3D12GetDebugInterfaceFn;
		static PFN_D3D12MACreateAllocator D3D12MACreateAllocatorFn;
		
	};

	class DXGIFunctions
	{
	public:
		static void LoadDXGIFunctions();
		static void UnloadDXGIFunctions();

		static void LoadMockDXGIFunctions();

		static bool IsLoaded();

		static bool s_IsLoaded;
		static PFN_CreateDXGIFactory2 CreateDXGIFactory2Fn;
		static PFN_DXGIGetDebugInterface1 DXGIGetDebugInterface1Fn;
	};
}

#endif
