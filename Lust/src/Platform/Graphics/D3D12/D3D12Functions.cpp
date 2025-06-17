#include "D3D12Functions.hpp"

bool Lust::D3D12Functions::s_IsLoaded = false;
PFN_D3D12CreateDevice Lust::D3D12Functions::D3D12CreateDeviceFn;
PFN_D3D12GetDebugInterface Lust::D3D12Functions::D3D12GetDebugInterfaceFn;
PFN_D3D12MACreateAllocator Lust::D3D12Functions::D3D12MACreateAllocatorFn;
bool Lust::DXGIFunctions::s_IsLoaded = false;
PFN_CreateDXGIFactory2 Lust::DXGIFunctions::CreateDXGIFactory2Fn;
PFN_DXGIGetDebugInterface1 Lust::DXGIFunctions::DXGIGetDebugInterface1Fn;

void Lust::D3D12Functions::LoadD3D12Functions()
{
	if (s_IsLoaded)
		return;

	D3D12CreateDeviceFn = D3D12CreateDevice;
	D3D12GetDebugInterfaceFn = D3D12GetDebugInterface;
	D3D12MACreateAllocatorFn = D3D12MA::CreateAllocator;
	s_IsLoaded = true;
}

void Lust::D3D12Functions::UnloadD3D12Functions()
{
	if (!s_IsLoaded)
		return;

	D3D12CreateDeviceFn = nullptr;
	D3D12GetDebugInterfaceFn = nullptr;
	D3D12MACreateAllocatorFn = nullptr;
	s_IsLoaded = false;
}

void Lust::D3D12Functions::LoadMockD3D12Functions()
{
	if (s_IsLoaded)
		return;
}

bool Lust::D3D12Functions::IsLoaded()
{
	return s_IsLoaded;
}

void Lust::DXGIFunctions::LoadDXGIFunctions()
{
	if (s_IsLoaded)
		return;

	CreateDXGIFactory2Fn = CreateDXGIFactory2;
	DXGIGetDebugInterface1Fn = DXGIGetDebugInterface1;
	s_IsLoaded = true;
}

void Lust::DXGIFunctions::UnloadDXGIFunctions()
{
	if (!s_IsLoaded)
		return;

	CreateDXGIFactory2Fn = nullptr;
	DXGIGetDebugInterface1Fn = nullptr;
	s_IsLoaded = false;
}

void Lust::DXGIFunctions::LoadMockDXGIFunctions()
{
	if (s_IsLoaded)
		return;
}

bool Lust::DXGIFunctions::IsLoaded()
{
	return s_IsLoaded;
}
