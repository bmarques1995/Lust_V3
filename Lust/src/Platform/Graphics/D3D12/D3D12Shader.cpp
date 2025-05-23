#ifdef LUST_USES_WINDOWS

#include "D3D12Shader.hpp"
#include "FileHandler.hpp"
#include "Application.hpp"
#include <filesystem>

namespace fs = std::filesystem;

const std::unordered_map<std::string, std::function<void(IDxcBlob**, D3D12_GRAPHICS_PIPELINE_STATE_DESC*)>> Lust::D3D12Shader::s_ShaderPusher =
{
	{"vs", [](IDxcBlob** blob, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)-> void
	{
		graphicsDesc->VS = {(*blob)->GetBufferPointer(), (*blob)->GetBufferSize()};
	}},
	{"ps", [](IDxcBlob** blob, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)-> void
	{
		graphicsDesc->PS = {(*blob)->GetBufferPointer(), (*blob)->GetBufferSize()};
	}},
	{"gs", [](IDxcBlob** blob, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)-> void
	{
		graphicsDesc->GS = {(*blob)->GetBufferPointer(), (*blob)->GetBufferSize()};
	}},
	{"ds", [](IDxcBlob** blob, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)-> void
	{
		graphicsDesc->DS = {(*blob)->GetBufferPointer(), (*blob)->GetBufferSize()};
	}},
	{"hs", [](IDxcBlob** blob, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)-> void
	{
		graphicsDesc->HS = {(*blob)->GetBufferPointer(), (*blob)->GetBufferSize()};
	}},
};

const std::list<std::string> Lust::D3D12Shader::s_GraphicsPipelineStages =
{
	"vs",
	"ps",
	"gs",
	"ds",
	"hs",
};

Lust::D3D12Shader::D3D12Shader(const D3D12Context* context, std::string json_controller_path, const std::shared_ptr<ShaderReflector>& inputInfo, const Topology& topology) :
	m_Context(context), Lust::Shader(inputInfo, json_controller_path)
{
	HRESULT hr;
	auto device = m_Context->GetDevicePtr();

	InitJsonAndPaths(json_controller_path, &(this->m_PipelineInfo), &(this->m_ShaderDir));
	StartDXC();
	m_RenderTopology = GetNativeTopology(topology);

	auto smallBuffers = m_ShaderReflector->GetSmallBufferLayout().GetElements();
	auto uniforms = m_ShaderReflector->GetUniformLayout().GetElements();
	auto structuredBuffers = m_ShaderReflector->GetStructuredBufferLayout().GetElements();

	auto nativeElements = m_ShaderReflector->GetInputLayout().GetElements();
	D3D12_INPUT_ELEMENT_DESC* ied = new D3D12_INPUT_ELEMENT_DESC[nativeElements.size()];

	for (size_t i = 0; i < nativeElements.size(); i++)
	{
		ied[i].SemanticName = nativeElements[i].GetName().c_str();
		ied[i].SemanticIndex = 0;
		ied[i].Format = GetNativeFormat(nativeElements[i].GetType());
		ied[i].InputSlot = 0;
		ied[i].AlignedByteOffset = nativeElements[i].GetOffset();
		ied[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		ied[i].InstanceDataStepRate = 0;
	}

	assert(m_RootSignatureSize <= 256);

	CreateGraphicsRootSignature(m_RootSignature.GetAddressOf(), device);

	//https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_pipeline_state_subobject_type
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsDesc = {};
	graphicsDesc.NodeMask = 1;
	graphicsDesc.InputLayout = { ied, (uint32_t)nativeElements.size() };
	graphicsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	graphicsDesc.pRootSignature = m_RootSignature.Get();
	graphicsDesc.SampleMask = UINT_MAX;
	graphicsDesc.NumRenderTargets = 1;
	graphicsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	graphicsDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	graphicsDesc.PrimitiveTopologyType = GetNativeTopologyType(topology);
	graphicsDesc.SampleDesc.Count = 1;
	graphicsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	size_t i = 0;
	for (auto& uniform : uniforms)
	{
		unsigned char* data = new unsigned char[uniform.second.GetSize()];
		if (uniform.second.GetAccessLevel() == AccessLevel::ROOT_BUFFER)
			PreallocateRootCBuffer(data, uniform.second);
		else
			PreallocateTabledCBuffer(data, uniform.second);
		delete[] data;
		i++;
	}

	i = 0;
	for (auto& structuredBuffer : structuredBuffers)
	{
		if (structuredBuffer.second.GetAccessLevel() == AccessLevel::ROOT_BUFFER)
			PreallocateRootSSBO(structuredBuffer.second);
		else
			PreallocateTabledSSBO(structuredBuffer.second);
		i++;
	}

	uint32_t samplersNumber = 0;
	uint32_t samplersRootSigIndex = 0xffffffff;

	if(m_ShaderReflector->GetSamplerLayout().GetElements().begin() != m_ShaderReflector->GetSamplerLayout().GetElements().end())
		samplersRootSigIndex = m_ShaderReflector->GetSamplerLayout().GetElements().begin()->second.GetShaderRegister();
	else
	{
		if (m_ShaderReflector->GetSamplerArrayLayout().GetElements().begin() != m_ShaderReflector->GetSamplerArrayLayout().GetElements().end())
		{
			samplersRootSigIndex = m_ShaderReflector->GetSamplerArrayLayout().GetElements().begin()->second.GetShaderRegister();
		}
	}

	samplersNumber+= m_ShaderReflector->GetSamplerLayout().GetElements().size();
	
	for (auto samplerArray : m_ShaderReflector->GetSamplerArrayLayout().GetElements())
	{
		samplersNumber += samplerArray.second.GetNumberOfSamplers();
	}

	uint32_t texturesNumber = 0;
	uint32_t texturesRootSigIndex = 0xffffffff;

	if (m_ShaderReflector->GetTextureLayout().GetElements().begin() != m_ShaderReflector->GetTextureLayout().GetElements().end())
		texturesRootSigIndex = m_ShaderReflector->GetTextureLayout().GetElements().begin()->second.GetShaderRegister();
	else
	{
		if (m_ShaderReflector->GetTextureArrayLayout().GetElements().begin() != m_ShaderReflector->GetTextureArrayLayout().GetElements().end())
		{
			texturesRootSigIndex = m_ShaderReflector->GetTextureArrayLayout().GetElements().begin()->second.GetShaderRegister();
		}
	}

	texturesNumber += m_ShaderReflector->GetTextureLayout().GetElements().size();

	for (auto textureArray : m_ShaderReflector->GetTextureArrayLayout().GetElements())
	{
		texturesNumber += textureArray.second.GetNumberOfTextures();
	}

	if(samplersNumber > 0)
		PreallocateSamplerDescriptors(samplersNumber, samplersRootSigIndex);

	if(texturesNumber > 0)
		PreallocateTextureDescriptors(texturesNumber, texturesRootSigIndex);

	
	for (auto& desc : m_TabledDescriptors)
		m_MergedHeaps.push_back(desc.second.Get());
	for (auto& desc : m_SamplerDescriptors)
		m_MergedHeaps.push_back(desc.second.Get());

	for (auto it = s_GraphicsPipelineStages.begin(); it != s_GraphicsPipelineStages.end(); it++)
	{
		PushShader(*it, &graphicsDesc);
	}

	BuildBlender(&graphicsDesc);
	BuildRasterizer(&graphicsDesc);
	BuildDepthStencil(&graphicsDesc);

	D3D12_PIPELINE_STATE_STREAM_DESC sample;

	hr = device->CreateGraphicsPipelineState(&graphicsDesc, IID_PPV_ARGS(m_GraphicsPipeline.GetAddressOf()));
	assert(hr == S_OK);

	delete[] ied;
}

Lust::D3D12Shader::~D3D12Shader()
{
}

void Lust::D3D12Shader::Stage()
{
	auto cmdList = m_Context->GetCurrentCommandList();
	cmdList->IASetPrimitiveTopology(m_RenderTopology);
	cmdList->SetGraphicsRootSignature(m_RootSignature.Get());
	cmdList->SetPipelineState(m_GraphicsPipeline.Get());
}

uint32_t Lust::D3D12Shader::GetStride() const
{
	return m_ShaderReflector->GetInputLayout().GetStride();
}

uint32_t Lust::D3D12Shader::GetOffset() const
{
	return 0;
}

void Lust::D3D12Shader::UploadTexture2D(const std::shared_ptr<Texture2D>* texture, const TextureElement& textureElement)
{
	CreateTextureSRV((const std::shared_ptr<D3D12Texture2D>*) texture, textureElement);
}

void Lust::D3D12Shader::UploadTexture2D(const std::shared_ptr<Texture2D>* texture, const TextureArray& textureArray, uint32_t offset)
{
	CreateTextureSRV((const std::shared_ptr<D3D12Texture2D>*) texture, textureArray, offset);
}

void Lust::D3D12Shader::UploadConstantBuffer(const std::shared_ptr<UniformBuffer>* buffer, const UniformElement& uploadCBV)
{
	if (uploadCBV.GetAccessLevel() == AccessLevel::ROOT_BUFFER)
	{
		CreateRootCBV((const std::shared_ptr<D3D12UniformBuffer>*) buffer, uploadCBV);
	}
	else
	{
		CreateTabledCBV((const std::shared_ptr<D3D12UniformBuffer>*) buffer, uploadCBV);
	}
}

void Lust::D3D12Shader::UploadStructuredBuffer(const std::shared_ptr<StructuredBuffer>* buffer, const StructuredBufferElement& uploadSRV)
{
	if (uploadSRV.GetAccessLevel() == AccessLevel::ROOT_BUFFER)
	{
		CreateRootSRV((const std::shared_ptr<D3D12StructuredBuffer>*) buffer, uploadSRV);
	}
	else
	{
		CreateTabledSRV((const std::shared_ptr<D3D12StructuredBuffer>*) buffer, uploadSRV);
	}
}

void Lust::D3D12Shader::UploadSampler(const std::shared_ptr<Sampler>* sampler, const SamplerElement& textureElement)
{
	CreateSamplerView((const std::shared_ptr<D3D12Sampler>*) sampler, textureElement);
}

void Lust::D3D12Shader::UploadSampler(const std::shared_ptr<Sampler>* sampler, const SamplerArray& samplerArray, uint32_t offset)
{
	CreateSamplerView((const std::shared_ptr<D3D12Sampler>*) sampler, samplerArray, offset);
}

void Lust::D3D12Shader::BindSmallBuffer(const void* data, size_t size, const SmallBufferElement& smallBuffer, size_t offset)
{
	if (size != smallBuffer.GetSize())
		throw SizeMismatchException(size, smallBuffer.GetSize());
	auto cmdList = m_Context->GetCurrentCommandList();
	auto smallStride = m_Context->GetSmallBufferAttachment();
	cmdList->SetGraphicsRoot32BitConstants(smallBuffer.GetBindingSlot(), size / smallStride, data, offset / smallStride);
}

void Lust::D3D12Shader::BindDescriptors()
{
	
	auto cmdList = m_Context->GetCurrentCommandList();
	for (auto& rootDescriptor : m_RootCBVDescriptors)
	{
		cmdList->SetGraphicsRootConstantBufferView(rootDescriptor.first, m_CBVAddresses[rootDescriptor.first]);
		//cmdList->SetGraphicsRootConstantBufferView(rootDescriptor.first, m_CBVResources[bufferLocation]->GetGPUVirtualAddress());
	}

	for (auto& rootDescriptor : m_RootSRVDescriptors)
	{
		cmdList->SetGraphicsRootShaderResourceView(rootDescriptor.first, m_SRVAddresses[rootDescriptor.first]);
	}

	//cmdList->SetGraphicsRootShaderResourceView

	if (m_MergedHeaps.size() > 0)
	{
		cmdList->SetDescriptorHeaps(m_MergedHeaps.size(), m_MergedHeaps.data());

		for (auto& tabledDescriptor : m_TabledDescriptors)
		{
			cmdList->SetGraphicsRootDescriptorTable(tabledDescriptor.first, tabledDescriptor.second->GetGPUDescriptorHandleForHeapStart());
		}
		for (auto& samplerDescriptor : m_SamplerDescriptors)
		{
			cmdList->SetGraphicsRootDescriptorTable(samplerDescriptor.first, samplerDescriptor.second->GetGPUDescriptorHandleForHeapStart());
		}
	}
}

void Lust::D3D12Shader::UploadTexturePackedDescSet(const TextureArray& textureArray)
{
}

void Lust::D3D12Shader::UploadSamplerPackedDescSet(const SamplerArray& samplerArray)
{
}

void Lust::D3D12Shader::StartDXC()
{
	HRESULT hr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(m_DxcLib.GetAddressOf()));
	assert(hr == S_OK);
}

void Lust::D3D12Shader::CreateRootCBV(const std::shared_ptr<D3D12UniformBuffer>* buffer, UniformElement uniformElement)
{
	auto device = m_Context->GetDevicePtr();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = (*buffer)->GetResource()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = uniformElement.GetSize();

	m_CBVAddresses[uniformElement.GetShaderRegister()] = cbvDesc.BufferLocation;

	device->CreateConstantBufferView(&cbvDesc, m_RootCBVDescriptors[uniformElement.GetShaderRegister()]->GetCPUDescriptorHandleForHeapStart());
}

void Lust::D3D12Shader::CreateTabledCBV(const std::shared_ptr<D3D12UniformBuffer>* buffer, UniformElement uniformElement)
{
	uint64_t bufferLocation = (((uint64_t)uniformElement.GetShaderRegister() << 32) + 1);
	auto device = m_Context->GetDevicePtr();

	D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle(m_TabledDescriptors[uniformElement.GetShaderRegister()]->GetCPUDescriptorHandleForHeapStart());
	UINT cbvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	cbvHandle.ptr += (cbvDescriptorSize * uniformElement.GetTableIndex());

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = (*buffer)->GetResource()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = uniformElement.GetSize();
	
	device->CreateConstantBufferView(&cbvDesc, cbvHandle);
}

void Lust::D3D12Shader::CreateRootSRV(const std::shared_ptr<D3D12StructuredBuffer>* buffer, StructuredBufferElement structuredBufferElement)
{
	auto device = m_Context->GetDevicePtr();

	m_SRVAddresses[structuredBufferElement.GetShaderRegister()] = (*buffer)->GetResource()->GetGPUVirtualAddress();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;  // Structured Buffers have no format
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = 1;
	srvDesc.Buffer.StructureByteStride = structuredBufferElement.GetSize();
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	device->CreateShaderResourceView((*buffer)->GetResource(), &srvDesc, m_RootSRVDescriptors[structuredBufferElement.GetShaderRegister()]->GetCPUDescriptorHandleForHeapStart());
}

void Lust::D3D12Shader::CreateTabledSRV(const std::shared_ptr<D3D12StructuredBuffer>* buffer, StructuredBufferElement uniformElement)
{
}

void Lust::D3D12Shader::CreateTextureSRV(const std::shared_ptr<D3D12Texture2D>* texture, const TextureElement& textureElement)
{
	auto device = m_Context->GetDevicePtr();

	auto srvHeapStartHandle = m_TabledDescriptors[textureElement.GetShaderRegister()]->GetCPUDescriptorHandleForHeapStart();
	UINT srvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHeapStartHandle.ptr += (textureElement.GetTextureIndex() * srvDescriptorSize);

	auto textureBufferDesc = (*texture)->GetResource()->GetDesc1();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureBufferDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0;
	srvDesc.Texture2D.PlaneSlice = 0;

	device->CreateShaderResourceView((*texture)->GetResource(), &srvDesc, srvHeapStartHandle);
}

void Lust::D3D12Shader::CreateTextureSRV(const std::shared_ptr<D3D12Texture2D>* texture, const TextureArray& textureArray, uint32_t offset)
{
	auto device = m_Context->GetDevicePtr();
	size_t descriptorOffset = textureArray.GetTextureIndex() + offset;

	auto srvHeapStartHandle = m_TabledDescriptors[textureArray.GetShaderRegister()]->GetCPUDescriptorHandleForHeapStart();
	UINT srvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHeapStartHandle.ptr += (descriptorOffset * srvDescriptorSize);

	auto textureBufferDesc = (*texture)->GetResource()->GetDesc1();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureBufferDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0;
	srvDesc.Texture2D.PlaneSlice = 0;

	device->CreateShaderResourceView((*texture)->GetResource(), &srvDesc, srvHeapStartHandle);
}

void Lust::D3D12Shader::CreateSamplerView(const std::shared_ptr<D3D12Sampler>* sampler, const SamplerElement& samplerElement)
{
	auto device = m_Context->GetDevicePtr();
	HRESULT hr;

	D3D12_SAMPLER_DESC samplerDesc = (*sampler)->GetSampler();

	auto samplerHeapStartHandle = m_SamplerDescriptors[samplerElement.GetShaderRegister()]->GetCPUDescriptorHandleForHeapStart();
	UINT samplerDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	samplerHeapStartHandle.ptr += (samplerElement.GetSamplerIndex() * samplerDescriptorSize);

	device->CreateSampler(&samplerDesc, samplerHeapStartHandle);
}

void Lust::D3D12Shader::CreateSamplerView(const std::shared_ptr<D3D12Sampler>* sampler, const SamplerArray& samplerArray, uint32_t offset)
{
	auto device = m_Context->GetDevicePtr();
	HRESULT hr;

	size_t descriptorOffset = samplerArray.GetSamplerIndex() + offset;

	D3D12_SAMPLER_DESC samplerDesc = (*sampler)->GetSampler();

	auto samplerHeapStartHandle = m_SamplerDescriptors[samplerArray.GetShaderRegister()]->GetCPUDescriptorHandleForHeapStart();
	UINT samplerDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	samplerHeapStartHandle.ptr += (descriptorOffset * samplerDescriptorSize);

	device->CreateSampler(&samplerDesc, samplerHeapStartHandle);
}

void Lust::D3D12Shader::PreallocateSamplerDescriptors(uint32_t numOfSamplers, uint32_t rootSigIndex)
{
	auto device = m_Context->GetDevicePtr();
	HRESULT hr;

	D3D12_DESCRIPTOR_HEAP_DESC srvDescriptorHeapDesc{};
	srvDescriptorHeapDesc.Type = GetNativeHeapType(BufferType::SAMPLER_BUFFER);
	srvDescriptorHeapDesc.NumDescriptors = numOfSamplers;
	srvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescriptorHeapDesc.NodeMask = 0;

	hr = device->CreateDescriptorHeap(&srvDescriptorHeapDesc, IID_PPV_ARGS(m_SamplerDescriptors[rootSigIndex].GetAddressOf()));
	assert(hr == S_OK);
}

void Lust::D3D12Shader::PreallocateTextureDescriptors(uint32_t numOfTextures, uint32_t rootSigIndex)
{	
	auto device = m_Context->GetDevicePtr();
	HRESULT hr;

	D3D12_DESCRIPTOR_HEAP_DESC srvDescriptorHeapDesc{};
	srvDescriptorHeapDesc.Type = GetNativeHeapType(BufferType::TEXTURE_BUFFER);
	srvDescriptorHeapDesc.NumDescriptors = numOfTextures;
	srvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescriptorHeapDesc.NodeMask = 0;

	hr = device->CreateDescriptorHeap(&srvDescriptorHeapDesc, IID_PPV_ARGS(m_TabledDescriptors[rootSigIndex].GetAddressOf()));
	assert(hr == S_OK);
}

bool Lust::D3D12Shader::IsBufferValid(size_t size)
{
	return ((size % m_Context->GetUniformAttachment()) == 0);
}

void Lust::D3D12Shader::PreallocateRootCBuffer(const void* data, UniformElement uniformElement)
{
	if (!IsBufferValid(uniformElement.GetSize()))
		throw AttachmentMismatchException(uniformElement.GetSize(), m_Context->GetSmallBufferAttachment());

	auto device = m_Context->GetDevicePtr();
	HRESULT hr;

	//m_CBVResources[bufferLocation] = nullptr;
	m_RootCBVDescriptors[uniformElement.GetShaderRegister()] = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC cbvDescriptorHeapDesc{};
	cbvDescriptorHeapDesc.Type = GetNativeHeapType(uniformElement.GetBufferType());
	cbvDescriptorHeapDesc.NumDescriptors = 1;
	cbvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvDescriptorHeapDesc.NodeMask = 0;

	hr = device->CreateDescriptorHeap(&cbvDescriptorHeapDesc, IID_PPV_ARGS(m_RootCBVDescriptors[uniformElement.GetShaderRegister()].GetAddressOf()));
	assert(hr == S_OK);

	D3D12_CPU_DESCRIPTOR_HANDLE cbvHeapStartHandle = m_RootCBVDescriptors[uniformElement.GetShaderRegister()]->GetCPUDescriptorHandleForHeapStart();

	device->CreateConstantBufferView(nullptr, cbvHeapStartHandle);
}

void Lust::D3D12Shader::PreallocateTabledCBuffer(const void* data, UniformElement uniformElement)
{
	if (!IsBufferValid(uniformElement.GetSize()))
		throw AttachmentMismatchException(uniformElement.GetSize(), m_Context->GetSmallBufferAttachment());

	auto device = m_Context->GetDevicePtr();
	HRESULT hr;

	// 4. Create the descriptor heap for CBVs
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = uniformElement.GetNumberOfBuffers(); // Two descriptors
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_TabledDescriptors[uniformElement.GetShaderRegister()].GetAddressOf()));
	assert(hr == S_OK);

	// 5. Create CBVs and place them in the descriptor heap
	D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle(m_TabledDescriptors[uniformElement.GetShaderRegister()]->GetCPUDescriptorHandleForHeapStart());
	UINT cbvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (UINT i = 0; i < uniformElement.GetNumberOfBuffers(); ++i)
	{
		// Create the CBV and advance the descriptor handle
		device->CreateConstantBufferView(nullptr, cbvHandle);
		cbvHandle.ptr += cbvDescriptorSize;

	}
}

void Lust::D3D12Shader::PreallocateRootSSBO(const StructuredBufferElement& structuredBufferElement)
{
	if (!IsBufferValid(structuredBufferElement.GetSize()))
		throw AttachmentMismatchException(structuredBufferElement.GetSize(), m_Context->GetSmallBufferAttachment());

	auto device = m_Context->GetDevicePtr();
	HRESULT hr;

	m_RootSRVDescriptors[structuredBufferElement.GetShaderRegister()] = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC srvDescriptorHeapDesc{};
	srvDescriptorHeapDesc.Type = GetNativeHeapType(structuredBufferElement.GetBufferType());
	srvDescriptorHeapDesc.NumDescriptors = 1;
	srvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescriptorHeapDesc.NodeMask = 0;

	hr = device->CreateDescriptorHeap(&srvDescriptorHeapDesc, IID_PPV_ARGS(m_RootSRVDescriptors[structuredBufferElement.GetShaderRegister()].GetAddressOf()));
	assert(hr == S_OK);

	D3D12_CPU_DESCRIPTOR_HANDLE srvHeapStartHandle = m_RootSRVDescriptors[structuredBufferElement.GetShaderRegister()]->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;  // Structured Buffers have no format
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = 1;
	srvDesc.Buffer.StructureByteStride = structuredBufferElement.GetSize();
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	device->CreateShaderResourceView(nullptr, &srvDesc, srvHeapStartHandle);
}

void Lust::D3D12Shader::PreallocateTabledSSBO(const StructuredBufferElement& structuredBufferElement)
{
	if (!IsBufferValid(structuredBufferElement.GetSize()))
		throw AttachmentMismatchException(structuredBufferElement.GetSize(), m_Context->GetSmallBufferAttachment());

	auto device = m_Context->GetDevicePtr();
	HRESULT hr;

	// 4. Create the descriptor heap for CBVs
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = structuredBufferElement.GetNumberOfElements();
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_TabledDescriptors[structuredBufferElement.GetShaderRegister()].GetAddressOf()));
	assert(hr == S_OK);

	// 5. Create CBVs and place them in the descriptor heap
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle(m_TabledDescriptors[structuredBufferElement.GetShaderRegister()]->GetCPUDescriptorHandleForHeapStart());
	UINT srvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (UINT i = 0; i < structuredBufferElement.GetNumberOfElements(); ++i)
	{

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;  // Structured Buffers have no format
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = 1;
		srvDesc.Buffer.StructureByteStride = structuredBufferElement.GetSize();
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		device->CreateShaderResourceView(nullptr, &srvDesc, srvHandle);
		
		srvHandle.ptr += srvDescriptorSize;
	}
}

void Lust::D3D12Shader::CreateGraphicsRootSignature(ID3D12RootSignature** rootSignature, ID3D12Device10* device)
{
	HRESULT hr;
	std::string shaderName = m_PipelineInfo["BinShaders"]["rs"]["filename"].asString();
	std::stringstream shaderFullPath;
	shaderFullPath << m_ShaderDir << "/" << shaderName;
	std::string shaderPath = shaderFullPath.str();

	if (!FileHandler::FileExists(shaderPath))
		return;

	ComPointer<IDxcUtils> lib;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(lib.GetAddressOf()));
	assert(hr == S_OK);

	size_t blobSize;
	std::byte* blobData;

	if (!FileHandler::ReadBinFile(shaderPath, &blobData, &blobSize))
		return;

	// Create blob from memory
	hr = lib->CreateBlob((void*)blobData, blobSize, DXC_CP_ACP, (IDxcBlobEncoding**)m_RootBlob.GetAddressOf());

	hr = device->CreateRootSignature(0, m_RootBlob->GetBufferPointer(), m_RootBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature));
	assert(hr == S_OK);

	delete[] blobData;
}

void Lust::D3D12Shader::BuildBlender(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)
{
	graphicsDesc->BlendState.AlphaToCoverageEnable = false;
	graphicsDesc->BlendState.RenderTarget[0].BlendEnable = true;
	graphicsDesc->BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	graphicsDesc->BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	graphicsDesc->BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	graphicsDesc->BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	graphicsDesc->BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	graphicsDesc->BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	graphicsDesc->BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void Lust::D3D12Shader::BuildRasterizer(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)
{
	graphicsDesc->RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	graphicsDesc->RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	graphicsDesc->RasterizerState.FrontCounterClockwise = false;
	graphicsDesc->RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	graphicsDesc->RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	graphicsDesc->RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	graphicsDesc->RasterizerState.DepthClipEnable = true;
	graphicsDesc->RasterizerState.MultisampleEnable = false;
	graphicsDesc->RasterizerState.AntialiasedLineEnable = false;
	graphicsDesc->RasterizerState.ForcedSampleCount = 0;
	graphicsDesc->RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

void Lust::D3D12Shader::BuildDepthStencil(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)
{
	graphicsDesc->DepthStencilState.DepthEnable = true;
	graphicsDesc->DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	graphicsDesc->DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	graphicsDesc->DepthStencilState.StencilEnable = true;
	graphicsDesc->DepthStencilState.FrontFace.StencilFailOp = graphicsDesc->DepthStencilState.FrontFace.StencilDepthFailOp = graphicsDesc->DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	graphicsDesc->DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	graphicsDesc->DepthStencilState.BackFace = graphicsDesc->DepthStencilState.FrontFace;
}

void Lust::D3D12Shader::PushShader(std::string_view stage, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)
{
	HRESULT hr;

	std::string shaderName = m_PipelineInfo["BinShaders"][stage.data()]["filename"].asString();
	if ((shaderName.size() == 0))
		return;
	std::stringstream shaderFullPath;
	shaderFullPath << m_ShaderDir << "/" << shaderName;
	std::string shaderPath = shaderFullPath.str();

	if (!FileHandler::FileExists(shaderPath))
		return;

	size_t blobSize;
	std::byte* blobData;

	if (!FileHandler::ReadBinFile(shaderPath, &blobData, &blobSize))
		return;

	// Create blob from memory
	ComPointer<IDxcBlob> pBlob;
	hr = m_DxcLib->CreateBlob((void*)blobData, blobSize, DXC_CP_ACP, (IDxcBlobEncoding**)pBlob.GetAddressOf());
	assert(hr == S_OK);
	m_ShaderBlobs[stage.data()] = pBlob;

	auto it = s_ShaderPusher.find(stage.data());
	if (it != s_ShaderPusher.end())
		it->second(m_ShaderBlobs[stage.data()].GetAddressOf(), graphicsDesc);

	delete[] blobData;
}

DXGI_FORMAT Lust::D3D12Shader::GetNativeFormat(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float: return DXGI_FORMAT_R32_FLOAT;
	case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
	case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
	case ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ShaderDataType::Uint: return DXGI_FORMAT_R32_UINT;
	case ShaderDataType::Uint2: return DXGI_FORMAT_R32G32_UINT;
	case ShaderDataType::Uint3: return DXGI_FORMAT_R32G32B32_UINT;
	case ShaderDataType::Uint4: return DXGI_FORMAT_R32G32B32A32_UINT;
	default: return DXGI_FORMAT_UNKNOWN;
	}
}

D3D12_DESCRIPTOR_HEAP_TYPE Lust::D3D12Shader::GetNativeHeapType(BufferType type)
{
	switch (type)
	{
	case BufferType::STRUCTURED_BUFFER:
	case BufferType::UNIFORM_CONSTANT_BUFFER:
	case BufferType::TEXTURE_BUFFER:
		return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	case BufferType::SAMPLER_BUFFER:
		return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	default:
	case BufferType::INVALID_BUFFER_TYPE:
		return D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
	}
}

D3D12_RESOURCE_DIMENSION Lust::D3D12Shader::GetNativeDimension(BufferType type)
{
	switch (type)
	{
	case BufferType::UNIFORM_CONSTANT_BUFFER:
	case BufferType::STRUCTURED_BUFFER:
		return D3D12_RESOURCE_DIMENSION_BUFFER;
	default:
	case BufferType::INVALID_BUFFER_TYPE:
		return D3D12_RESOURCE_DIMENSION_UNKNOWN;
	}
}

D3D12_FILTER Lust::D3D12Shader::GetNativeFilter(SamplerFilter filter)
{
	switch (filter)
	{
	default:
	case SamplerFilter::LINEAR:
		return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	case SamplerFilter::NEAREST:
		return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	case SamplerFilter::ANISOTROPIC:
		return D3D12_FILTER_ANISOTROPIC;
	}
}

D3D12_TEXTURE_ADDRESS_MODE Lust::D3D12Shader::GetNativeAddressMode(AddressMode addressMode)
{
	switch (addressMode)
	{
	default:
	case AddressMode::REPEAT:
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	case AddressMode::MIRROR:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	case AddressMode::CLAMP:
		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	case AddressMode::BORDER:
		return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	case AddressMode::MIRROR_ONCE:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
	}
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE Lust::D3D12Shader::GetNativeTopologyType(Topology topology)
{
	switch (topology)
	{
	case Lust::Topology::LUST_TOPOLOGY_POINTLIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	case Lust::Topology::LUST_TOPOLOGY_LINELIST:
	case Lust::Topology::LUST_TOPOLOGY_LINELIST_ADJ:
	case Lust::Topology::LUST_TOPOLOGY_LINESTRIP:
	case Lust::Topology::LUST_TOPOLOGY_LINESTRIP_ADJ:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	case Lust::Topology::LUST_TOPOLOGY_TRIANGLELIST:
	case Lust::Topology::LUST_TOPOLOGY_TRIANGLELIST_ADJ:
	case Lust::Topology::LUST_TOPOLOGY_TRIANGLESTRIP:
	case Lust::Topology::LUST_TOPOLOGY_TRIANGLESTRIP_ADJ:
	case Lust::Topology::LUST_TOPOLOGY_TRIANGLE_FAN:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	default:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}
}

D3D12_PRIMITIVE_TOPOLOGY Lust::D3D12Shader::GetNativeTopology(Topology topology)
{
	switch (topology)
	{
	case Lust::Topology::LUST_TOPOLOGY_POINTLIST:
		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case Lust::Topology::LUST_TOPOLOGY_LINELIST:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case Lust::Topology::LUST_TOPOLOGY_LINELIST_ADJ:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	case Lust::Topology::LUST_TOPOLOGY_LINESTRIP:
		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case Lust::Topology::LUST_TOPOLOGY_LINESTRIP_ADJ:
		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
	case Lust::Topology::LUST_TOPOLOGY_TRIANGLELIST:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case Lust::Topology::LUST_TOPOLOGY_TRIANGLELIST_ADJ:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
	case Lust::Topology::LUST_TOPOLOGY_TRIANGLESTRIP:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case Lust::Topology::LUST_TOPOLOGY_TRIANGLESTRIP_ADJ:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
	case Lust::Topology::LUST_TOPOLOGY_TRIANGLE_FAN:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLEFAN;
	default:
		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
}

#endif
