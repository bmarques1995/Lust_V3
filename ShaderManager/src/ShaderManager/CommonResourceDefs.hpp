#pragma once

#include "LustShaderManagerDLLMacro.hpp"

namespace Lust
{
	/**
	* @brief Enum for buffer types
	* @param INVALID_BUFFER_TYPE represents no buffer type
	* @param UNIFORM_CONSTANT_BUFFER represents a uniform buffer(vulkan), or constant buffer(d3d12)
	* @param STRUCTURED_BUFFER represents a storage buffer(vulkan), or structured buffer(d3d12)
	* @param TEXTURE_BUFFER represents a texture buffer
	* @param SAMPLER_BUFFER represents a sampler buffer
	*/
	enum class LUST_SHADER_MNG_API BufferType
	{
		INVALID_BUFFER_TYPE,
		UNIFORM_CONSTANT_BUFFER,
		STRUCTURED_BUFFER,
		TEXTURE_BUFFER,
		SAMPLER_BUFFER
	};

	/**
	* @brief Enum for access levels, this is used to determine if the buffer is a root buffer or a descriptor buffer in d3d12
	* @param ROOT_BUFFER represents a root buffer
	* @param DESCRIPTOR_BUFFER represents a descriptor buffer
	*/
	enum class AccessLevel
	{
		ROOT_BUFFER,
		DESCRIPTOR_BUFFER
	};

	/**
	* @brief Allowed stages
	* @param EMPTY_STAGE represents no stage
	* @param VERTEX_STAGE represents the vertex stage
	* @param PIXEL_STAGE represents the fragment/pixel stage
	* @param GEOMETRY_STAGE represents the geometry stage
	* @param HULL_STAGE represents the tessellation control/hull stage
	* @param DOMAIN_STAGE represents the tessellation evaluation/domain stage
	* @param MESH_STAGE represents the mesh stage
	* @param AMPLIFICATION_STAGE represents the task/amplification stage
	* @param RAYTRACING_STAGE represents the raytracing stage
	*/
	enum LUST_SHADER_MNG_API AllowedStages
	{
		EMPTY_STAGE = 1 >> 1,
		VERTEX_STAGE = 1 << 0,
		PIXEL_STAGE = 1 << 1,
		GEOMETRY_STAGE = 1 << 2,
		HULL_STAGE = 1 << 3,
		DOMAIN_STAGE = 1 << 4,
		MESH_STAGE = 1 << 5,
		AMPLIFICATION_STAGE = 1 << 6,
		RAYTRACING_STAGE = 1 << 7
	};
}