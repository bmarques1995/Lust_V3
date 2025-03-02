#pragma once

#ifdef LUST_USES_WINDOWS

#include "Shader.hpp"
#include "D3D12Context.hpp"
#include "D3D12Texture.hpp"
#include "D3D12Buffer.hpp"
#include "DXCSafeInclude.hpp"
#include <functional>

namespace Lust
{
	struct ResourceAndHeap
	{
		ComPointer<ID3D12Resource2> Resource;
		ComPointer<ID3D12DescriptorHeap> Heap;
	};

	class LUST_API D3D12Shader : public Shader
	{
	public:
		D3D12Shader(const D3D12Context* context, std::string json_controller_path, InputInfo inputInfo);
		~D3D12Shader();

		void Stage() override;
		virtual uint32_t GetStride() const override;
		virtual uint32_t GetOffset() const override;

		void UploadTexture2D(const std::shared_ptr<Texture2D>* texture, const TextureElement& textureElement) override;

		void UploadConstantBuffer(const std::shared_ptr<UniformBuffer>* buffer, const UniformElement& uploadCBV) override;

		void UploadStructuredBuffer(const std::shared_ptr<StructuredBuffer>* buffer, const StructuredBufferElement& uploadSRV) override;

		void BindSmallBuffer(const void* data, size_t size, uint32_t bindingSlot, size_t offset) override;

		void BindDescriptors() override;

		void CreateSampler(SamplerElement samplerElement) override;

	private:
		void StartDXC();

		void CreateRootCBV(const std::shared_ptr<D3D12UniformBuffer>* buffer, UniformElement uniformElement);
		void CreateTabledCBV(const std::shared_ptr<D3D12UniformBuffer>* buffer, UniformElement uniformElement);
		void CreateRootSRV(const std::shared_ptr<D3D12StructuredBuffer>* buffer, StructuredBufferElement uniformElement);
		void CreateTabledSRV(const std::shared_ptr<D3D12StructuredBuffer>* buffer, StructuredBufferElement uniformElement);

		void CreateTextureSRV(const std::shared_ptr<D3D12Texture2D>* texture, const TextureElement& textureElement);
		void PreallocateSamplerDescriptors(uint32_t numOfSamplers, uint32_t rootSigIndex);
		void PreallocateTextureDescriptors(uint32_t numOfTextures, uint32_t rootSigIndex);

		void CreateBuffer(size_t bufferSize, DXGI_FORMAT format, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_DIMENSION dimension, ID3D12Resource2** buffer);

		bool IsBufferValid(size_t size);
		void PreallocateRootCBuffer(const void* data, UniformElement uniformElement);
		void PreallocateTabledCBuffer(const void* data, UniformElement uniformElement);

		void PreallocateRootSSBO(const StructuredBufferElement& structuredBufferElement);
		void PreallocateTabledSSBO(const StructuredBufferElement& structuredBufferElement);

		void CreateGraphicsRootSignature(ID3D12RootSignature** rootSignature, ID3D12Device10* device);
		void BuildBlender(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);
		void BuildRasterizer(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);
		void BuildDepthStencil(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);

		void PushShader(std::string_view stage, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);

		static DXGI_FORMAT GetNativeFormat(ShaderDataType type);
		static D3D12_DESCRIPTOR_HEAP_TYPE GetNativeHeapType(BufferType type);
		static D3D12_RESOURCE_DIMENSION GetNativeDimension(BufferType type);
		static D3D12_FILTER GetNativeFilter(SamplerFilter filter);
		static D3D12_TEXTURE_ADDRESS_MODE GetNativeAddressMode(AddressMode addressMode);

		static D3D12_PRIMITIVE_TOPOLOGY_TYPE GetNativeTopologyType(Topology topology);
		static D3D12_PRIMITIVE_TOPOLOGY GetNativeTopology(Topology topology);

		static const std::unordered_map<std::string, std::function<void(IDxcBlob**, D3D12_GRAPHICS_PIPELINE_STATE_DESC*)>> s_ShaderPusher;
		static const std::list<std::string> s_GraphicsPipelineStages;


		Json::Value m_PipelineInfo;

		std::unordered_map<uint64_t, D3D12_GPU_VIRTUAL_ADDRESS> m_CBVAddresses;
		std::unordered_map<uint64_t, D3D12_GPU_VIRTUAL_ADDRESS> m_SRVAddresses;
		//std::unordered_map<uint64_t, ComPointer<ID3D12Resource2>> m_CBVResources;
		//std::unordered_map<uint64_t, ComPointer<ID3D12Resource2>> m_SSBOResources;

		const D3D12Context* m_Context;
		
		D3D12_PRIMITIVE_TOPOLOGY m_RenderTopology;
		ComPointer<IDxcUtils> m_DxcLib;
		ComPointer<IDxcBlob> m_RootBlob;
		ComPointer<ID3D12PipelineState> m_GraphicsPipeline;
		std::unordered_map<std::string, ComPointer<IDxcBlob>> m_ShaderBlobs;
		
		ComPointer<ID3D12RootSignature> m_RootSignature;
		std::unordered_map<uint32_t, ComPointer<ID3D12DescriptorHeap>> m_RootCBVDescriptors;
		std::unordered_map<uint32_t, ComPointer<ID3D12DescriptorHeap>> m_RootSRVDescriptors;
		std::unordered_map<uint32_t, ComPointer<ID3D12DescriptorHeap>> m_TabledDescriptors;
		std::unordered_map<uint32_t, ComPointer<ID3D12DescriptorHeap>> m_SamplerDescriptors;
		std::vector<ID3D12DescriptorHeap*> m_MergedHeaps;

		/*in bytes*/
		uint32_t m_RootSignatureSize = 0;
	};
}

#endif
