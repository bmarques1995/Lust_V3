#ifdef LUST_USES_WINDOWS

#include "D3D12Context.hpp"
#include "D3D12Functions.hpp"
#include <cassert>

const std::unordered_map<uint32_t, D3D_FEATURE_LEVEL> Lust::D3D12Context::s_AvaliableFeatureLevels =
{
	{0, D3D_FEATURE_LEVEL_11_0},
	{1, D3D_FEATURE_LEVEL_11_1},
	{2, D3D_FEATURE_LEVEL_12_0},
	{3, D3D_FEATURE_LEVEL_12_1},
	{4, D3D_FEATURE_LEVEL_12_2},
};

Lust::D3D12Context::D3D12Context(const Window* windowHandle, uint32_t framesInFlight) :
	m_FramesInFlight(framesInFlight), m_IsVSyncEnabled(true)
{
	if(!D3D12Functions::IsLoaded())
		D3D12Functions::LoadD3D12Functions();
	if (!DXGIFunctions::IsLoaded())
		DXGIFunctions::LoadDXGIFunctions();
	//#27ae60
	SetClearColor(0x27 / 255.0f, 0xae / 255.0f, 0x60 / 255.0f, 1.0f);

#ifdef LUST_DEBUG_MODE
	EnableDebug();
#endif

	CreateFactory();
	CreateAdapter();
	CreateDevice();
	CreateInfoQueue();
	CreateAllocator();
	CreateCommandQueue();
	CreateViewportAndScissor(windowHandle->GetWidth(), windowHandle->GetHeight());
	CreateSwapChain(std::any_cast<HWND>(windowHandle->GetNativePointer()));
	CreateRenderTargetView();
	CreateDepthStencilView();
	CreateCommandAllocator();
	CreateCommandLists();
}

Lust::D3D12Context::~D3D12Context()
{
	FlushQueue();
	m_CommandQueueFence.Release();
	for (size_t i = 0; i < m_FramesInFlight; i++)
	{
		m_CommandLists[i].Release();
		m_CommandAllocators[i].Release();
	}
	delete[] m_CommandLists;
	delete[] m_CommandAllocators;
	m_DepthStencilView.Release();
	m_DSVAllocation.Release();
	m_DSVHeap.Release();
	m_DSVHandle = {};
	for (size_t i = 0; i < m_FramesInFlight; i++)
	{
		m_RenderTargets[i].Release();
		m_RTVHandles[i] = {};
	}
	delete[] m_RenderTargets;
	delete[] m_RTVHandles;
	m_RTVHeap.Release();
	m_SwapChain.Release();
	m_CommandQueue.Release();
	m_Allocator.Release();
	m_InfoQueue.Release();
	m_Device.Release();

	m_DXGIAdapter.Release();
	m_DXGIFactory.Release();
#ifdef LUST_DEBUG_MODE
	DisableDebug();
#endif

	if(D3D12Functions::IsLoaded())
		D3D12Functions::UnloadD3D12Functions();
	if (DXGIFunctions::IsLoaded())
		DXGIFunctions::UnloadDXGIFunctions();
}

void Lust::D3D12Context::SetClearColor(float r, float g, float b, float a)
{
	m_ClearColor.Color[0] = r;
	m_ClearColor.Color[1] = g;
	m_ClearColor.Color[2] = b;
	m_ClearColor.Color[3] = a;
}

uint32_t Lust::D3D12Context::GetUniformAttachment() const
{
	return 256;
}

uint32_t Lust::D3D12Context::GetSmallBufferAttachment() const
{
	return 4;
}

uint32_t Lust::D3D12Context::GetFramesInFlight() const
{
	return m_FramesInFlight;
}

void Lust::D3D12Context::FillRenderPass()
{
	auto backBuffer = m_RenderTargets[m_CurrentBufferIndex];
	auto rtvHandle = m_RTVHandles[m_CurrentBufferIndex];
	auto dsvHandle = m_DSVHandle;

	D3D12_RESOURCE_BARRIER rtSetupBarrier{};
	rtSetupBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	rtSetupBarrier.Transition.pResource = backBuffer.Get();
	rtSetupBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	rtSetupBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	rtSetupBarrier.Transition.Subresource = 0;
	rtSetupBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	m_CommandLists[m_CurrentBufferIndex]->ResourceBarrier(1, &rtSetupBarrier);

	D3D12_RENDER_PASS_RENDER_TARGET_DESC renderTargetDesc = {};
	renderTargetDesc.cpuDescriptor = rtvHandle;
	renderTargetDesc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
	renderTargetDesc.BeginningAccess.Clear.ClearValue = m_ClearColor;
	renderTargetDesc.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

	D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.cpuDescriptor = m_DSVHandle;
	depthStencilDesc.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
	depthStencilDesc.DepthBeginningAccess.Clear.ClearValue.DepthStencil.Depth = 1.0f;
	depthStencilDesc.DepthBeginningAccess.Clear.ClearValue.DepthStencil.Stencil = 0;
	depthStencilDesc.DepthBeginningAccess.Clear.ClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.DepthEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

	m_CommandLists[m_CurrentBufferIndex]->BeginRenderPass(1, &renderTargetDesc, &depthStencilDesc, D3D12_RENDER_PASS_FLAG_NONE);
}

void Lust::D3D12Context::SubmitRenderPass()
{
	auto backBuffer = m_RenderTargets[m_CurrentBufferIndex];

	D3D12_RESOURCE_BARRIER rtSetupBarrier{};
	rtSetupBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	rtSetupBarrier.Transition.pResource = backBuffer.Get();
	rtSetupBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	rtSetupBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	rtSetupBarrier.Transition.Subresource = 0;
	rtSetupBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	m_CommandLists[m_CurrentBufferIndex]->EndRenderPass();

	m_CommandLists[m_CurrentBufferIndex]->ResourceBarrier(1, &rtSetupBarrier);
}

void Lust::D3D12Context::ReceiveCommands()
{
	m_CurrentBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
	m_Allocator->SetCurrentFrameIndex(m_CurrentBufferIndex);
}

void Lust::D3D12Context::DispatchCommands()
{
	// === Execute commands ===
	auto hr = m_CommandLists[m_CurrentBufferIndex]->Close();

	ID3D12CommandList* lists[] = { m_CommandLists[m_CurrentBufferIndex].Get() };
	m_CommandQueue->ExecuteCommandLists(1, lists);

	FlushQueue();

	m_CommandAllocators[m_CurrentBufferIndex]->Reset();
	m_CommandLists[m_CurrentBufferIndex]->Reset(m_CommandAllocators[m_CurrentBufferIndex].Get(), nullptr);
}

void Lust::D3D12Context::Present()
{
	m_SwapChain->Present(m_IsVSyncEnabled ? 1 : 0, m_IsVSyncEnabled ? 0 : DXGI_PRESENT_ALLOW_TEARING);
}

void Lust::D3D12Context::StageViewportAndScissors()
{
	m_CommandLists[m_CurrentBufferIndex]->RSSetViewports(1, &m_Viewport);
	m_CommandLists[m_CurrentBufferIndex]->RSSetScissorRects(1, &m_ScissorRect);
}

void Lust::D3D12Context::SetVSync(bool enableVSync)
{
	m_IsVSyncEnabled = enableVSync;
}

bool Lust::D3D12Context::IsVSyncEnabled() const
{
	return m_IsVSyncEnabled;
}


Lust::ComPointer<ID3D12Device14> Lust::D3D12Context::GetDevicePtr() const
{
	return m_Device;
}

Lust::ComPointer<D3D12MA::Allocator> Lust::D3D12Context::GetAllocatorPtr() const
{ 
	return m_Allocator;
}

Lust::ComPointer<ID3D12GraphicsCommandList10> Lust::D3D12Context::GetCurrentCommandList() const
{
	return m_CommandLists[m_CurrentBufferIndex].GetConst();
}

Lust::ComPointer<ID3D12CommandQueue> Lust::D3D12Context::GetCommandQueue() const
{
	return m_CommandQueue;
}

D3D_FEATURE_LEVEL Lust::D3D12Context::GetFeatureLevel() const
{
	return m_FeatureLevel;
}

const std::string& Lust::D3D12Context::GetGPUName()
{
	DXGI_ADAPTER_DESC gpuDescription;
	m_DXGIAdapter->GetDesc(&gpuDescription);
	m_GPUName.reserve(128);
	WideCharToMultiByte(CP_UTF8, 0, gpuDescription.Description, -1, m_GPUName.data(), 128, nullptr, nullptr);
	return m_GPUName;
}

void Lust::D3D12Context::WindowResize(uint32_t width, uint32_t height)
{
	FlushQueue(m_FramesInFlight);
	CreateViewportAndScissor(width, height);
	for (size_t i = 0; i < m_FramesInFlight; i++)
		m_RenderTargets[i].Release();
	m_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
	GetTargets();
	m_DepthStencilView.Release();
	m_DSVAllocation.Release();
	CreateDepthStencilView();
}

void Lust::D3D12Context::CreateFactory()
{
	HRESULT hr;
	UINT dxgiFactoryFlag = 0;
#ifdef LUST_DEBUG_MODE
	dxgiFactoryFlag = DXGI_CREATE_FACTORY_DEBUG;
#endif
	hr = DXGIFunctions::CreateDXGIFactory2Fn(dxgiFactoryFlag, IID_PPV_ARGS(m_DXGIFactory.GetAddressOf()));
	assert(hr == S_OK);
}

void Lust::D3D12Context::CreateAdapter()
{
	HRESULT hr = S_OK;

#if 0

	std::vector<std::pair<ComPointer<IDXGIAdapter4>, DXGI_ADAPTER_DESC3>> adapters;

	for (UINT adapterIndex = 0;; ++adapterIndex) {
		ComPointer<IDXGIAdapter4> adapter;
		if (m_DXGIFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.GetAddressOf())) == DXGI_ERROR_NOT_FOUND) {
			break; // No more adapters to enumerate.
		}

		DXGI_ADAPTER_DESC3 desc;
		adapter->GetDesc3(&desc);
		adapters.push_back(std::make_pair(adapter, desc));
	}

	auto adapterType = adapters[0].second;

#endif

	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_DXGIFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(m_DXGIAdapter.GetAddressOf())); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC3 desc;
		m_DXGIAdapter->GetDesc3(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
		{
			// Don't select the Basic Render Driver adapter.
			continue;
		}

		for (size_t i = 0; i < s_AvaliableFeatureLevels.size(); i++)
		{
			uint32_t index = s_AvaliableFeatureLevels.size() - 1 - i;
			auto it = s_AvaliableFeatureLevels.find(index);
			if (it != s_AvaliableFeatureLevels.end())
			{
				hr = D3D12Functions::D3D12CreateDeviceFn(m_DXGIAdapter.Get(), it->second, _uuidof(ID3D12Device), nullptr);
				if ((hr == S_OK) || (hr == S_FALSE))
				{
					m_FeatureLevel = it->second;
					break;
				}
			}
			
		}

		if ((hr == S_OK) || (hr == S_FALSE))
		{
			break;
		}
	}
	assert((hr == S_OK) || (hr == S_FALSE));
}

void Lust::D3D12Context::CreateDevice()
{
	D3D12Functions::D3D12CreateDeviceFn(m_DXGIAdapter.Get(), m_FeatureLevel, IID_PPV_ARGS(m_Device.GetAddressOf()));
	m_Device->SetName(L"Full device");
}

void Lust::D3D12Context::CreateInfoQueue()
{
	HRESULT hr;
	D3D12_INFO_QUEUE_FILTER filter = {};
	D3D12_MESSAGE_ID denyIds[] = {
		D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED
	};
	filter.DenyList.NumIDs = _countof(denyIds);
	filter.DenyList.pIDList = denyIds;

	hr = m_Device->QueryInterface(m_InfoQueue.GetAddressOf());
	m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, FALSE);
	m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, FALSE);
	m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE);
	m_InfoQueue->AddStorageFilterEntries(&filter);
	assert(hr == S_OK);
}

void Lust::D3D12Context::CreateAllocator()
{
	HRESULT hr;

	D3D12MA::ALLOCATOR_DESC desc = {};
	desc.Flags = D3D12MA::ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED;
	desc.pDevice = m_Device;
	desc.pAdapter = m_DXGIAdapter;

	hr = D3D12Functions::D3D12MACreateAllocatorFn(&desc, m_Allocator.GetAddressOf());
	assert(hr == S_OK);
}

void Lust::D3D12Context::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;
	m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf()));

	m_Device->CreateFence(m_CommandQueueFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_CommandQueueFence.GetAddressOf()));

	m_CommandQueueFenceEvent = CreateEventW(nullptr, false, false, nullptr);
}

void Lust::D3D12Context::CreateSwapChain(HWND windowHandle)
{
	DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = (uint32_t)m_ScissorRect.right;
	swapChainDesc.Height = (uint32_t)m_ScissorRect.bottom;
	swapChainDesc.Format = swapChainFormat;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = m_FramesInFlight;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

	m_ClearColor.Format = swapChainFormat;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc{};
	fullscreenDesc.RefreshRate.Denominator = 90;
	fullscreenDesc.RefreshRate.Numerator = 1;
	fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	fullscreenDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	fullscreenDesc.Windowed = TRUE;

	ComPointer<IDXGISwapChain1> swapChain;
	m_DXGIFactory->CreateSwapChainForHwnd(m_CommandQueue.Get(), windowHandle, &swapChainDesc, &fullscreenDesc, nullptr, &swapChain);
	//m_DXGIFactory->CreateSwapChainForHwnd(m_CommandQueue.Get(), windowHandle, &swapChainDesc, nullptr, nullptr, &swapChain);
	swapChain->QueryInterface(IID_PPV_ARGS(m_SwapChain.GetAddressOf()));
	m_SwapChain->SetMaximumFrameLatency(m_FramesInFlight);
}

void Lust::D3D12Context::CreateRenderTargetView()
{
	// === Get heap metrics ===

	m_RTVHeapIncrement = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// === Retrive RTV & Buffers ===

	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NumDescriptors = m_FramesInFlight;
	rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorHeapDesc.NodeMask = 0;

	m_Device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(m_RTVHeap.GetAddressOf()));

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapStartHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
	m_RTVHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[m_FramesInFlight];
	for (size_t i = 0; i < m_FramesInFlight; i++)
	{
		m_RTVHandles[i] = { rtvHeapStartHandle.ptr + i * m_RTVHeapIncrement };
	}

	m_RenderTargets = new ComPointer<ID3D12Resource2>[m_FramesInFlight];
	GetTargets();
}

void Lust::D3D12Context::CreateCommandAllocator()
{
	m_CommandAllocators = new ComPointer<ID3D12CommandAllocator>[m_FramesInFlight];
	for (size_t i = 0; i < m_FramesInFlight; i++)
	{
		m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocators[i].GetAddressOf()));
	}

}

void Lust::D3D12Context::CreateCommandLists()
{
	m_CommandLists = new ComPointer<ID3D12GraphicsCommandList10>[m_FramesInFlight];
	for (size_t i = 0; i < m_FramesInFlight; i++)
	{
		m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocators[i].Get(), nullptr, IID_PPV_ARGS(m_CommandLists[i].GetAddressOf()));
	}
}

void Lust::D3D12Context::CreateViewportAndScissor(uint32_t width, uint32_t height)
{
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = (float)width;
	m_Viewport.Height = (float)height;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_ScissorRect.left = 0;
	m_ScissorRect.right = (long)width;
	m_ScissorRect.top = 0;
	m_ScissorRect.bottom = (long)height;
}

void Lust::D3D12Context::CreateDepthStencilView()
{
	HRESULT hr;

	// === Retrive RTV & Buffers ===

	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc{};
	dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorHeapDesc.NumDescriptors = 1;
	dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvDescriptorHeapDesc.NodeMask = 0;

	hr = m_Device->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(m_DSVHeap.GetAddressOf()));
	assert(hr == S_OK);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHeapStartHandle = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
	m_DSVHandle = { dsvHeapStartHandle.ptr };

	D3D12_RESOURCE_DESC1 depthStencilDesc = {};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Width = m_ScissorRect.right;
	depthStencilDesc.Height = m_ScissorRect.bottom;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	D3D12MA::ALLOCATION_DESC allocDesc = {};
	allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

	hr = m_Allocator->CreateResource2(
		&allocDesc, &depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, nullptr,
		m_DSVAllocation.GetAddressOf(), IID_PPV_ARGS(m_DepthStencilView.GetAddressOf()));
	assert(hr == S_OK);

	// Create the depth/stencil view
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_Device->CreateDepthStencilView(m_DepthStencilView.Get(), &dsvDesc, m_DSVHandle);
}

void Lust::D3D12Context::GetTargets()
{
	HRESULT hr;
	for (size_t i = 0; i < m_FramesInFlight; i++)
	{
		hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_RenderTargets[i].GetAddressOf()));
		assert(hr == S_OK);
		m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, m_RTVHandles[i]);
	}
}

void Lust::D3D12Context::FlushQueue(size_t flushCount)
{
	for (size_t i = 0; i < flushCount; i++)
	{
		m_CommandQueue->Signal(m_CommandQueueFence.Get(), ++m_CommandQueueFenceValue);
		WaitForFence();
	}
}

void Lust::D3D12Context::WaitForFence(UINT64 fenceValue)
{
	HRESULT hr;
	if (fenceValue == -1) fenceValue = m_CommandQueueFenceValue;

	if (m_CommandQueueFence->GetCompletedValue() < fenceValue)
	{
		hr = m_CommandQueueFence->SetEventOnCompletion(fenceValue, m_CommandQueueFenceEvent);
		if (hr == S_OK)
			if (WaitForSingleObject(m_CommandQueueFenceEvent, INFINITE) == WAIT_OBJECT_0)
				return;
		// Fallback wait
		while (m_CommandQueueFence->GetCompletedValue() < fenceValue) Sleep(1);
	}
	auto waitable = m_SwapChain->GetFrameLatencyWaitableObject();
	WaitForMultipleObjects(1, &waitable, TRUE, INFINITE);
	CloseHandle(waitable);
}

#ifdef LUST_DEBUG_MODE

void Lust::D3D12Context::EnableDebug()
{
	DXGIFunctions::DXGIGetDebugInterface1Fn(0, IID_PPV_ARGS(m_DXGIDebug.GetAddressOf()));
	D3D12Functions::D3D12GetDebugInterfaceFn(IID_PPV_ARGS(m_D3D12Debug.GetAddressOf()));

	m_D3D12Debug->EnableDebugLayer();
}

void Lust::D3D12Context::DisableDebug()
{
	m_D3D12Debug.Release();
	m_DXGIDebug->EnableLeakTrackingForThread();
	m_DXGIDebug->ReportLiveObjects(
		DXGI_DEBUG_ALL,
		(DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL)
	);
	m_DXGIDebug.Release();
}

#endif

#endif
