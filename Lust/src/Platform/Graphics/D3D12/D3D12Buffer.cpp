#ifdef LUST_USES_WINDOWS

#include "D3D12Buffer.hpp"
#include <cassert>

Lust::D3D12Buffer::D3D12Buffer(const D3D12Context* context) :
	m_Context(context)
{
}

void Lust::D3D12Buffer::CreateBuffer(const void* data, size_t size)
{
	HRESULT hr;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC1 resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = (UINT)size;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	auto device = m_Context->GetDevicePtr();
	
	D3D12_RANGE readRange = { 0 };
	hr = device->CreateCommittedResource2(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		nullptr,
		IID_PPV_ARGS(m_Buffer.GetAddressOf()));
	assert(hr == S_OK);

	void* gpuData = nullptr;
	hr = m_Buffer->Map(0, &readRange, &gpuData);
	assert(hr == S_OK);
	memcpy(gpuData, data, size);
	m_Buffer->Unmap(0, NULL);
}

void Lust::D3D12Buffer::DestroyBuffer()
{
	m_Buffer.Release();
}

Lust::D3D12VertexBuffer::D3D12VertexBuffer(const D3D12Context* context, const void* data, size_t size, uint32_t stride) :
	D3D12Buffer(context)
{
	CreateBuffer(data, size);

	memset(&m_VertexBufferView, 0, sizeof(D3D12_VERTEX_BUFFER_VIEW));
	m_VertexBufferView.BufferLocation = m_Buffer->GetGPUVirtualAddress();
	m_VertexBufferView.SizeInBytes = size;
	m_VertexBufferView.StrideInBytes = stride;
}

Lust::D3D12VertexBuffer::~D3D12VertexBuffer()
{
	DestroyBuffer();
}

void Lust::D3D12VertexBuffer::Stage() const
{
	auto cmdList = m_Context->GetCurrentCommandList();
	cmdList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
}

Lust::D3D12IndexBuffer::D3D12IndexBuffer(const D3D12Context* context, const void* data, size_t count) :
	D3D12Buffer(context)
{
	CreateBuffer(data, count * sizeof(uint32_t));
	m_Count = (uint32_t)count;

	m_IndexBufferView.BufferLocation = m_Buffer->GetGPUVirtualAddress();
	m_IndexBufferView.SizeInBytes = count * sizeof(uint32_t);
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

Lust::D3D12IndexBuffer::~D3D12IndexBuffer()
{
	DestroyBuffer();
}

void Lust::D3D12IndexBuffer::Stage() const
{
	auto cmdList = m_Context->GetCurrentCommandList();
	cmdList->IASetIndexBuffer(&m_IndexBufferView);
}

uint32_t Lust::D3D12IndexBuffer::GetCount() const
{
	return m_Count;
}

#endif