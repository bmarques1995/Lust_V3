#ifdef LUST_USES_WINDOWS

#include "D3D12Texture.hpp"
#include <cassert>
#include "D3D12CopyPipeline.hpp"
#include "Application.hpp"
#include <vector>

Lust::D3D12Texture2D::D3D12Texture2D(const D3D12Context* context, const TextureBuffer& specification) :
	Texture2D(specification), m_Context(context), m_Specification(specification)
{
	m_Loaded = false;
	CreateResource();
	CopyBuffer();
	m_Loaded = true;
	m_Specification.FreeImage();
}

Lust::D3D12Texture2D::~D3D12Texture2D()
{
}

const Lust::TextureBuffer& Lust::D3D12Texture2D::GetTextureDescription() const
{
	return m_Specification;
}

uint32_t Lust::D3D12Texture2D::GetWidth() const
{
	return m_Specification.GetWidth();
}

uint32_t Lust::D3D12Texture2D::GetHeight() const
{
	return m_Specification.GetHeight();
}

bool Lust::D3D12Texture2D::IsLoaded() const
{
	return m_Loaded;
}

ID3D12Resource2* Lust::D3D12Texture2D::GetResource() const
{
	return m_Texture.GetConst();
}

void Lust::D3D12Texture2D::UpdateTextureInfo(const D3D12_RESOURCE_DESC1& desc)
{
	m_Specification.m_Width = desc.Width;
	m_Specification.m_Height = desc.Height;
	m_Specification.m_Depth = desc.DepthOrArraySize;
	m_Specification.m_MipsLevel = desc.MipLevels;
	m_Specification.m_Channels = 4;
}

void Lust::D3D12Texture2D::CreateResource()
{
	auto device = m_Context->GetDevicePtr();
	HRESULT hr;

	D3D12_RESOURCE_DESC1 textureBufferDesc = {};
	textureBufferDesc.Dimension = GetNativeTensor(m_Specification.GetTensor());
	textureBufferDesc.Alignment = m_Specification.GetAlignment() == ImageAlignment::PERFECT ? 0 : 65536;
	textureBufferDesc.Width = m_Specification.GetWidth(); //mandatory
	textureBufferDesc.Height = m_Specification.GetHeight(); // mandatory 2 and 3
	textureBufferDesc.DepthOrArraySize = m_Specification.GetDepth(); // mandatory 3
	textureBufferDesc.MipLevels = m_Specification.GetMipsLevel(); // param
	textureBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureBufferDesc.SampleDesc.Count = 1;
	textureBufferDesc.SampleDesc.Quality = 0;
	textureBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	hr = device->CreateCommittedResource2(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&textureBufferDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		nullptr,
		IID_PPV_ARGS(m_Texture.GetAddressOf()));

	assert(hr == S_OK);
}

void Lust::D3D12Texture2D::CopyBuffer()
{
	HRESULT hr;
	auto device = m_Context->GetDevicePtr();
	ComPointer<ID3D12Resource2> textureBuffer;
	std::shared_ptr<D3D12CopyPipeline>* copyPipeline = (std::shared_ptr<D3D12CopyPipeline>*)
		TextureLibrary::GetCopyPipeline();

	auto copyCommandList = (*copyPipeline)->GetCommandList();
	auto copyCommandAllocator = (*copyPipeline)->GetCommandAllocator();
	auto copyCommandQueue = (*copyPipeline)->GetCommandQueue();

	D3D12_RESOURCE_DESC uploadBufferDesc = {};
	uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadBufferDesc.Width = (m_Specification.GetWidth() * m_Specification.GetHeight() * m_Specification.GetDepth() * m_Specification.GetChannels());
	uploadBufferDesc.Height = 1;
	uploadBufferDesc.DepthOrArraySize = 1;
	uploadBufferDesc.MipLevels = 1;
	uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	uploadBufferDesc.SampleDesc.Count = 1;
	uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_HEAP_PROPERTIES uploadHeapProperties = {};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	hr = device->CreateCommittedResource1(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&uploadBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,  // Upload heap is CPU-accessible
		nullptr,
		nullptr,
		IID_PPV_ARGS(textureBuffer.GetAddressOf())
	);

	assert(hr == S_OK);

	D3D12_RANGE readRange = { 0 };
	void* gpuData = nullptr;
	hr = textureBuffer->Map(0, &readRange, &gpuData);
	assert(hr == S_OK);
	size_t textureSize = (m_Specification.GetWidth() * m_Specification.GetHeight() * m_Specification.GetDepth() * m_Specification.GetChannels());
	memcpy(gpuData, m_Specification.GetTextureBuffer(), textureSize);
	textureBuffer->Unmap(0, NULL);

	D3D12_TEXTURE_COPY_LOCATION destLocation = {};
	destLocation.pResource = m_Texture;
	destLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	destLocation.SubresourceIndex = 0;

	D3D12_RESOURCE_DESC1 textureDesc = m_Texture->GetDesc1();

	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = textureBuffer.Get();
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	//3rd arg represents the number of subresources
	device->GetCopyableFootprints1(&textureDesc, 0, 1, 0, &srcLocation.PlacedFootprint, nullptr, nullptr, nullptr);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = m_Texture;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	copyCommandList->ResourceBarrier(1, &barrier);

	copyCommandList->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);

	// Transition the resource to PIXEL_SHADER_RESOURCE for sampling

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = m_Texture;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	copyCommandList->ResourceBarrier(1, &barrier);

	copyCommandList->Close();

	ID3D12CommandList* lists[] = { copyCommandList };
	copyCommandQueue->ExecuteCommandLists(1, lists);

	(*copyPipeline)->Wait();

	copyCommandAllocator->Reset();
	copyCommandList->Reset(copyCommandAllocator, nullptr);
}

D3D12_RESOURCE_DIMENSION Lust::D3D12Texture2D::GetNativeTensor(TextureTensor tensor)
{
	switch (tensor)
	{
	case TextureTensor::TENSOR_1:
		return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
	case TextureTensor::TENSOR_2:
		return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	case TextureTensor::TENSOR_3:
		return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	default:
		return D3D12_RESOURCE_DIMENSION_UNKNOWN;
	}
}

#endif
