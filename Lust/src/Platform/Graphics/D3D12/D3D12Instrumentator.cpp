#ifdef LUST_USES_WINDOWS

#include "D3D12Instrumentator.hpp"
#include <cassert>
#include <iostream>

Lust::D3D12Instrumentator::D3D12Instrumentator(const D3D12Context* context) :
	m_Context(context), m_QueryCount(2)
{
	HRESULT hr;
	auto device = m_Context->GetDevicePtr();

	D3D12_QUERY_HEAP_DESC queryHeapDesc = {};
	queryHeapDesc.Count = m_QueryCount;  // Number of queries (e.g., 2 for a start and end timestamp)
	queryHeapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;  // Timestamp queries

	hr = device->CreateQueryHeap(&queryHeapDesc, IID_PPV_ARGS(&m_QueryHeap));
	assert(hr == S_OK);

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC1 bufferDesc = {};

	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Alignment = 0;
	bufferDesc.Width = 8;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.SampleDesc.Quality = 0;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	hr = device->CreateCommittedResource2(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		nullptr,
		IID_PPV_ARGS(m_QueryResult.GetAddressOf()));
	assert(hr == S_OK);

	heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_READBACK;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	hr = device->CreateCommittedResource2(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		nullptr,
		IID_PPV_ARGS(m_QueryVisibleBuffer.GetAddressOf()));
	assert(hr == S_OK);

	auto cmdQueue = m_Context->GetCommandQueue();
	cmdQueue->GetTimestampFrequency(&m_Frequency);
}

Lust::D3D12Instrumentator::~D3D12Instrumentator()
{
}

void Lust::D3D12Instrumentator::BeginQueryTime()
{
	auto cmdQueue = m_Context->GetCommandQueue();
	uint64_t CPUTickReference = 0;
	cmdQueue->GetClockCalibration(&m_LastTime, &CPUTickReference);
}

void Lust::D3D12Instrumentator::EndQueryTime()
{
	auto cmdList = m_Context->GetCurrentCommandList();
	cmdList->EndQuery(m_QueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 0);
	D3D12_RESOURCE_BARRIER barrier[2];

	barrier[0] = {};
	barrier[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier[0].Transition.pResource = m_QueryResult.Get();
	barrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	barrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	
	cmdList->ResourceBarrier(1, barrier);
	cmdList->ResolveQueryData(m_QueryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0, 1, m_QueryResult.Get(), 0);

	barrier[0] = {};
	barrier[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier[0].Transition.pResource = m_QueryResult.Get();
	barrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
	barrier[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	
	barrier[1] = {};
	barrier[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier[1].Transition.pResource = m_QueryVisibleBuffer.Get();
	barrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	barrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	cmdList->ResourceBarrier(2, barrier);

	cmdList->CopyResource(m_QueryVisibleBuffer, m_QueryResult);

	barrier[0] = {};
	barrier[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier[0].Transition.pResource = m_QueryResult.Get();
	barrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
	barrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
	barrier[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	barrier[1] = {};
	barrier[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier[1].Transition.pResource = m_QueryVisibleBuffer.Get();
	barrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
	barrier[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	cmdList->ResourceBarrier(2, barrier);
}

double Lust::D3D12Instrumentator::GetQueryTime()
{
	HRESULT hr;
	void* mappedData;
	hr = m_QueryVisibleBuffer->Map(0, nullptr, &mappedData);

	memcpy(&m_Timestamp, mappedData, 8);

	m_QueryVisibleBuffer->Unmap(0, nullptr);

	double result = 0.0;

	if (hr == S_OK)
	{
		uint64_t elapsedTime = m_Timestamp - m_LastTime;
		result = (elapsedTime / (double)m_Frequency); // Convert to seconds	
	}

	return result;
}

#endif
