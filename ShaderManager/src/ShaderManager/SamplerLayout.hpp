#pragma once

#include "LustShaderManagerDLLMacro.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <initializer_list>
#include "CommonResourceDefs.hpp"

namespace Lust
{
	/**
	* @brief Enum for sampler filter types
	* @param LINEAR represents linear filtering
	* @param NEAREST represents nearest/point filtering
	* @param ANISOTROPIC represents anisotropic filtering
	*/
	enum class LUST_SHADER_MNG_API SamplerFilter
	{
		LINEAR,
		NEAREST,
		ANISOTROPIC
	};

	/**
	* @brief Enum for anisotropic factor types
	* @param FACTOR_0 represents anisotropic filtering level 1
	* @param FACTOR_1 represents anisotropic filtering level 2
	* @param FACTOR_2 represents anisotropic filtering level 4
	* @param FACTOR_3 represents anisotropic filtering level 8
	* @param FACTOR_4 represents anisotropic filtering level 16
	*/
	enum class LUST_SHADER_MNG_API AnisotropicFactor
	{
		FACTOR_0,
		FACTOR_1,
		FACTOR_2,
		FACTOR_3,
		FACTOR_4,
	};

	/**
	* @brief Enum for address mode types
	* @param REPEAT represents repeat
	* @param MIRROR represents mirror
	* @param CLAMP represents clamp
	* @param BORDER represents border
	* @param MIRROR_ONCE represents mirror once
	*/
	enum class LUST_SHADER_MNG_API AddressMode
	{
		REPEAT,
		MIRROR,
		CLAMP,
		BORDER,
		MIRROR_ONCE
	};

	/**
	* @brief Enum for comparison pass mode types
	* @param NEVER represents never
	* @param LESS represents less
	* @param EQUAL represents equal
	* @param LESS_EQUAL represents less equal
	* @param GREATER represents greater
	* @param NEQ represents not equal
	* @param GREATER_EQUAL represents greater equal
	* @param ALWAYS represents always
	*/
	enum class LUST_SHADER_MNG_API ComparisonPassMode
	{
		NEVER,
		LESS,
		EQUAL,
		LESS_EQUAL,
		GREATER,
		NEQ,
		GREATER_EQUAL,
		ALWAYS
	};

	/**
	* @brief Class for sampler elements
	*/
	class LUST_SHADER_MNG_API SamplerElement
	{
		friend class SamplerLayout;
	public:
		/**
		* @brief Default constructor
		*/
		SamplerElement();
		/**
		* @brief Constructor
		* @param bindingSlot binding slot (Vulkan only), "binding", e.g. binding = 0
		* @param spaceSet descriptor set (Vulkan only), "set", e.g. set = 0
		* @param shaderRegister root signature element that describes the sampler (D3D12 only), "root Signature Index",
		* @param samplerIndex offset in the root signature (D3D12 only), "sampler Index", e.g. samplerIndex = 0 means the first sampler in the root signature
		*/
		SamplerElement(uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t samplerIndex, const std::string& name);

		/**
		* @brief Returns the name of the sampler
		*/
		const std::string& GetName() const;
		
		/**
		* @brief Returns the binding slot
		*/
		uint32_t GetBindingSlot() const;
		/**
		* @brief Returns the space set
		*/
		uint32_t GetSpaceSet() const;
		/**
		* @brief Returns the shader register
		*/
		uint32_t GetShaderRegister() const;
		/**
		* @brief Returns the sampler index
		*/
		uint32_t GetSamplerIndex() const;

	private:
		std::string m_Name;
		
		uint32_t m_BindingSlot;
		uint32_t m_SpaceSet;
		uint32_t m_ShaderRegister;
		uint32_t m_SamplerIndex;
	};

	/**
	* @brief Container for sampler elements
	*/
	class LUST_SHADER_MNG_API SamplerLayout
	{
	public:
		/**
		* @brief Default constructor
		* @param elements Elements of the layout
		*/
		SamplerLayout(std::initializer_list<SamplerElement> elements);

		/**
		* @brief Returns the element of the layout
		* @param name name(key) of the element
		* @return element of the layout, with the name or an empty element
		*/
		const SamplerElement& GetElement(std::string name) const;
		/**
		* @brief Returns the elements of the layout
		* @return elements of the layout
		*/
		const std::unordered_map<std::string, SamplerElement>& GetElements() const;

		/**
		* @brief Clears the layout
		*/
		void Clear();
		/**
		* @brief Uploads the element to the layout
		*/
		void Upload(const SamplerElement& element);
	private:
		std::unordered_map<std::string, SamplerElement> m_Samplers;
		static SamplerElement s_EmptyElement;
	};

	/**
	* @brief Container for sampler arrays, sampler array is a variable that retains an array of samplers
	*/
	class LUST_SHADER_MNG_API SamplerArray
	{
		friend class SamplerArrayLayout;
	public:

		/**
		* @brief Default constructor
		*/
		SamplerArray();
		/**
		* @brief Constructor
		* @param bindingSlot binding slot (Vulkan only), "binding", e.g. binding = 0
		* @param spaceSet descriptor set (Vulkan only), "set", e.g. set = 0
		* @param shaderRegister root signature element that describes the sampler (D3D12 only), "root Signature Index",
		* @param samplerIndex offset in the root signature of the first element (D3D12 only), e.g. samplerIndex = 256 means the first sampler has an offset of 256 elements
		* @param numberOfSamplers number of samplers in the array
		*/
		SamplerArray(uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t samplerIndex, uint32_t numberOfSamplers, const std::string& name);

		/**
		* @brief Returns the name of the sampler array
		*/
		const std::string& GetName() const;

		/**
		* @brief Returns the binding slot
		*/
		uint32_t GetBindingSlot() const;
		/**
		* @brief Returns the space set
		*/
		uint32_t GetSpaceSet() const;
		/**
		* @brief Returns the shader register
		*/
		uint32_t GetShaderRegister() const;
		/**
		* @brief Returns the sampler index
		*/
		uint32_t GetSamplerIndex() const;
		/**
		* @brief Returns the number of samplers
		*/
		uint32_t GetNumberOfSamplers() const;

	private:
		std::string m_Name;

		uint32_t m_NumberOfSamplers;
		uint32_t m_BindingSlot;
		uint32_t m_SpaceSet;
		uint32_t m_ShaderRegister;
		uint32_t m_SamplerIndex;
	};

	/**
	* @brief Container for sampler arrays
	*/
	class LUST_SHADER_MNG_API SamplerArrayLayout
	{
	public:
		/**
		* @brief Default constructor
		* @param elements elements of the layout
		*/
		SamplerArrayLayout(std::initializer_list<SamplerArray> elements);

		/**
		* @brief Returns the element of the layout
		* @param name name(key) of the element
		* @return element of the layout, with the name or an empty element
		*/
		const SamplerArray& GetElement(std::string name) const;
		/**
		* @brief Returns the elements of the layout
		* @return elements of the layout
		*/
		const std::unordered_map<std::string, SamplerArray>& GetElements() const;

		/**
		* @brief Clears the layout
		*/
		void Clear();
		/**
		* @brief Uploads the element to the layout
		*/
		void Upload(const SamplerArray& element);
	private:
		std::unordered_map<std::string, SamplerArray> m_Samplers;
		static SamplerArray s_EmptyElement;
	};

	/**
	* @brief Container for sampler info, that describes the sampler
	*/
	class LUST_SHADER_MNG_API SamplerInfo
	{
	public:
		/**
		* @brief Default constructor
		*/
		SamplerInfo();
		/**
		* @brief Constructor
		* @param filter filter of the sampler
		* @param anisotropicFactor anisotropic factor of the sampler
		* @param addressMode address mode of the sampler
		* @param comparisonPassMode comparison mode of the sampler
		*/
		SamplerInfo(SamplerFilter filter, AnisotropicFactor anisotropicFactor, AddressMode addressMode, ComparisonPassMode comparisonPassMode);

		/**
		* @brief Returns the filter of the sampler
		*/
		SamplerFilter GetFilter() const;
		/**
		* @brief Returns the anisotropic factor of the sampler
		*/
		AnisotropicFactor GetAnisotropicFactor() const;
		/**
		* @brief Returns the address mode of the sampler
		*/
		AddressMode GetAddressMode() const;
		/**
		* @brief Returns the comparison mode of the sampler
		*/
		ComparisonPassMode GetComparisonPassMode() const;

	private:
		SamplerFilter m_Filter;
		AnisotropicFactor m_AnisotropicFactor;
		AddressMode m_AddressMode;
		ComparisonPassMode m_ComparisonPassMode;
	};
}
