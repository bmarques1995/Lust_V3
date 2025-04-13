#ifdef LUST_USES_WINDOWS

#include "D3D12Buffer.hpp"
#include <cassert>
#include "UniformsLayout.hpp"

ID3D12Resource2* Lust::D3D12Buffer::GetResource() const
{
	return m_Buffer.GetConst();
}

Lust::D3D12Buffer::D3D12Buffer(const D3D12Context* context) :
	m_Context(context), m_GPUData(nullptr)
{
}

void Lust::D3D12Buffer::CreateBuffer(const void* data, size_t size, bool dynamicBuffer)
{
	HRESULT hr;
	m_IsDynamic = dynamicBuffer;

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
	
	hr = device->CreateCommittedResource2(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		nullptr,
		IID_PPV_ARGS(m_Buffer.GetAddressOf()));
	assert(hr == S_OK);

	if (m_IsDynamic)
	{
		D3D12_RANGE readRange = { 0 };
		hr = m_Buffer->Map(0, &readRange, (void**)&m_GPUData);
		assert(hr == S_OK);
	}
	RemapCall(data, size, 0);
}

void Lust::D3D12Buffer::DestroyBuffer()
{
	if (m_IsDynamic)
	{
		m_Buffer->Unmap(0, NULL);
	}
	m_Buffer.Release();
}

void Lust::D3D12Buffer::RemapBuffer(const void* data, size_t size, size_t offset)
{
	memcpy(m_GPUData + offset, data, size);
}

void Lust::D3D12Buffer::RemapBufferStaticly(const void* data, size_t size, size_t offset)
{
	HRESULT hr;
	D3D12_RANGE readRange = { 0 };
	hr = m_Buffer->Map(0, &readRange, (void**)&m_GPUData);
	assert(hr == S_OK);
	memcpy(m_GPUData + offset, data, size);
	m_Buffer->Unmap(0, NULL);
}

void Lust::D3D12Buffer::RemapCall(const void* data, size_t size, size_t offset)
{
	if (m_IsDynamic)
		RemapBuffer(data, size, offset);
	else
		RemapBufferStaticly(data, size, offset);
}

bool Lust::D3D12Buffer::IsBufferConformed(size_t size)
{
	return ((size % m_Context->GetUniformAttachment()) == 0);
}

Lust::D3D12VertexBuffer::D3D12VertexBuffer(const D3D12Context* context, const void* data, size_t size, uint32_t stride, bool dynamicBuffer) :
	D3D12Buffer(context)
{
	m_Stride = stride;
	CreateBuffer(data, size, dynamicBuffer);

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

void Lust::D3D12VertexBuffer::Remap(const void* data, size_t size)
{
	RemapCall(data, size, 0);
}

Lust::D3D12IndexBuffer::D3D12IndexBuffer(const D3D12Context* context, const void* data, uint32_t count, bool dynamicBuffer) :
	D3D12Buffer(context)
{
	CreateBuffer(data, count * sizeof(uint32_t), dynamicBuffer);
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

void Lust::D3D12IndexBuffer::Remap(const void* data, uint32_t count)
{
	RemapCall(data, count * sizeof(uint32_t), 0);
}

Lust::D3D12UniformBuffer::D3D12UniformBuffer(const D3D12Context* context, const void* data, size_t size) :
	D3D12Buffer(context)
{
	m_BufferSize = size;
	if (!IsBufferConformed(size))
		throw AttachmentMismatchException(size, m_Context->GetUniformAttachment());
	CreateBuffer(data, size);
	Remap(data, size);
}

Lust::D3D12UniformBuffer::~D3D12UniformBuffer()
{
	DestroyBuffer();
}

void Lust::D3D12UniformBuffer::Remap(const void* data, size_t size)
{
	RemapCall(data, size, 0);
}

size_t Lust::D3D12UniformBuffer::GetSize() const
{
	return m_BufferSize;
}

Lust::D3D12StructuredBuffer::D3D12StructuredBuffer(const D3D12Context* context, const void* data, size_t size) :
	D3D12Buffer(context)
{
	m_BufferSize = size;
	if (!IsBufferConformed(size))
	{
		size_t bufferCorrection = size;
		bufferCorrection += (m_Context->GetUniformAttachment() - (bufferCorrection % m_Context->GetUniformAttachment()));
		m_BufferSize += bufferCorrection;
	}
	CreateBuffer(data, size);
	Remap(data, size, 0);
}

Lust::D3D12StructuredBuffer::~D3D12StructuredBuffer()
{
	DestroyBuffer();
}

void Lust::D3D12StructuredBuffer::Remap(const void* data, size_t size, size_t offset)
{
	RemapCall(data, size, offset);
}

size_t Lust::D3D12StructuredBuffer::GetSize() const
{
	return m_BufferSize;
}

#endif
