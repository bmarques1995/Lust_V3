#ifdef LUST_USES_WINDOWS

#include "D3D12CopyPipeline.hpp"
#include <cassert>

Lust::D3D12CopyPipeline::D3D12CopyPipeline(const D3D12Context* context) :
	m_Context(context)
{
	auto device = m_Context->GetDevicePtr();
	HRESULT hr;

	m_CopyFenceEvent = CreateEventW(nullptr, false, false, nullptr);

	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;
	hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CopyCommandQueue.GetAddressOf()));
	assert(hr == S_OK);

	hr = device->CreateFence(m_CopyFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_CopyFence.GetAddressOf()));
	assert(hr == S_OK);

	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CopyCommandAllocator.GetAddressOf()));
	assert(hr == S_OK);

	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CopyCommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CopyCommandList.GetAddressOf()));
	assert(hr == S_OK);

	device->Release();
}

Lust::D3D12CopyPipeline::~D3D12CopyPipeline()
{
	m_CopyCommandList.Release();
	m_CopyCommandAllocator.Release();
	m_CopyCommandQueue.Release();
	m_Context = nullptr;
}

void Lust::D3D12CopyPipeline::Wait()
{
	WaitCopyPipeline();
	m_CopyFenceEvent = CreateEventW(nullptr, false, false, nullptr);
}

ID3D12GraphicsCommandList10* Lust::D3D12CopyPipeline::GetCommandList() const
{
	return m_CopyCommandList.GetConst();
}

ID3D12CommandAllocator* Lust::D3D12CopyPipeline::GetCommandAllocator() const
{
	return m_CopyCommandAllocator.GetConst();
}

ID3D12CommandQueue* Lust::D3D12CopyPipeline::GetCommandQueue() const
{
	return m_CopyCommandQueue.GetConst();
}

void Lust::D3D12CopyPipeline::WaitCopyPipeline(UINT64 fenceValue)
{
	HRESULT hr;

	m_CopyCommandQueue->Signal(m_CopyFence.Get(), ++m_CopyFenceValue);

	bool continueWaiting = true;

	if (fenceValue == -1)
		fenceValue = m_CopyFenceValue;

	if ((m_CopyFence->GetCompletedValue() < fenceValue) && continueWaiting)
	{
		hr = m_CopyFence->SetEventOnCompletion(fenceValue, m_CopyFenceEvent);
		if (hr == S_OK)
			continueWaiting = (WaitForSingleObject(m_CopyFenceEvent, INFINITE) != WAIT_OBJECT_0);
		// Fallback wait
		while (m_CopyFence->GetCompletedValue() < fenceValue) Sleep(1);
	}

	CloseHandle(m_CopyFenceEvent);
}


#endif