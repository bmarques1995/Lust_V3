#pragma once

#include "ShaderReflector.hpp"
#include <ComPointer.hpp>
#include "DXCSafeInclude.hpp"
#include <d3d12shader.h>

namespace Lust
{
	class D3D12ShaderReflector : public ShaderReflector
	{
	public:
		D3D12ShaderReflector(std::string_view jsonFilepath);
		~D3D12ShaderReflector();
	
	private:
		void UploadBlob(std::string_view shader_stage, IDxcBlob** blob);
		void StartDxc();

		void ReflectRootBlob();
		void ReflectStage(std::string_view shader_stage, IDxcBlob* shader_blob);

		void GenerateInputBufferLayout();

		static ShaderDataType CastToShaderDataType(int8_t mask, D3D_REGISTER_COMPONENT_TYPE type);

		ComPointer<IDxcUtils> m_DxcLib;
		ComPointer<IDxcContainerReflection> m_DxcContainerReflection = nullptr;

		ComPointer<IDxcBlob> m_RootBlob;
		std::unordered_map<std::string, ComPointer<IDxcBlob>> m_ShaderBlobs;

		ComPointer<ID3D12RootSignatureDeserializer> m_RootSigDeserializer;
		std::unordered_map<std::string, ComPointer<ID3D12ShaderReflection>> m_ShaderReflections;
	
		static const std::unordered_map<std::string, bool> s_HLSLSysVals;
	};
}