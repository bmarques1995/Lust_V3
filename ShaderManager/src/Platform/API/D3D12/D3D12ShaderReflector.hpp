#pragma once

#ifdef LUST_SHADER_MNG_USES_WINDOWS

#include "ShaderReflector.hpp"
#include <ComPointer.hpp>
#include "DXCSafeInclude.hpp"
#include <d3d12.h>
#include <d3d12shader.h>

namespace Lust
{
	class D3D12ShaderReflector : public ShaderReflector
	{
	public:
		D3D12ShaderReflector(std::string_view jsonFilepath, uint32_t stages, uint32_t numInstances);
		~D3D12ShaderReflector();
	
	private:
		void UploadBlob(std::string_view shader_stage, IDxcBlob** blob);
		void StartDxc();

		void ReflectRootBlob();
		void ReflectStage(std::string_view shader_stage, IDxcBlob* shader_blob);
		void ReflectRootSignature();

		void GenerateInputBufferLayout();

		void PushExternalElementPreInfo(ID3D12ShaderReflection* reflection, size_t resourceIndex);
		void PushRootElement(const D3D12_ROOT_PARAMETER& param, uint32_t rootIndex);

		void PushDescriptorTable(const D3D12_ROOT_PARAMETER& param, uint32_t rootIndex);

		static ShaderDataType CastToShaderDataType(int8_t mask, D3D_REGISTER_COMPONENT_TYPE type);

		ComPointer<IDxcUtils> m_DxcLib;
		ComPointer<IDxcContainerReflection> m_DxcContainerReflection = nullptr;

		ComPointer<IDxcBlob> m_RootBlob;
		std::unordered_map<std::string, ComPointer<IDxcBlob>> m_ShaderBlobs;

		ComPointer<ID3D12RootSignatureDeserializer> m_RootSigDeserializer;
		std::unordered_map<std::string, ComPointer<ID3D12ShaderReflection>> m_ShaderReflections;
	
		std::unordered_map<uint32_t, D3D12_SHADER_INPUT_BIND_DESC> m_BuffersMap;
		std::unordered_map<uint32_t, D3D12_SHADER_BUFFER_DESC> m_BuffersMapSizes;
		std::unordered_map<uint32_t, std::string> m_BuffersMapName;

		static const std::unordered_map<std::string, bool> s_HLSLSysVals;
		static const std::unordered_map<D3D_SHADER_INPUT_TYPE, uint16_t> s_TypesMap;
		static const std::unordered_map<D3D12_ROOT_PARAMETER_TYPE, uint16_t> s_ReturnTypesMap;
		static const std::unordered_map<D3D12_DESCRIPTOR_RANGE_TYPE, uint16_t> s_ReturnRangeTypesMap;
	};
}

#endif
