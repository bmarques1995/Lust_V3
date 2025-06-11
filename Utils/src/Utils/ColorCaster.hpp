#pragma once

#include "LustUtilsDLLMacro.hpp"
#include "MathComponents.hpp"

namespace Lust
{
	class LUST_UTILS_API ColorCaster
	{
	public:
		/**
			* @brief casts a float color to a uint32_t color, the color is in the format 0xRRGGBBAA, will be replicated to all vertices
			* @param color The color to cast
			*/
		static uvec4 CastFloatColor(const vec3& color);

		/**
		* @brief casts a float color to a uint32_t color, the color is in the format 0xRRGGBBAA, will be replicated to all vertices
		* @param color The color to cast
		*/
		static uvec4 CastFloatColor(const vec4& color);

		/**
		* @brief casts a float color to a uint32_t color, the color is in the format 0xRRGGBBAA, each vertex will have a different color
		* @param colorEdge00 The color of the first vertex
		* @param colorEdge01 The color of the second vertex
		* @param colorEdge10 The color of the third vertex
		* @param colorEdge11 The color of the fourth vertex
		*/
		static uvec4 CastFloatColor(const vec3& colorEdge00, const vec3& colorEdge01, const vec3& colorEdge10, const vec3& colorEdge11);

		/**
		* @brief casts a float color to a uint32_t color, the color is in the format 0xRRGGBBAA, each vertex will have a different color
		* @param colorEdge00 The color of the first vertex
		* @param colorEdge01 The color of the second vertex
		* @param colorEdge10 The color of the third vertex
		* @param colorEdge11 The color of the fourth vertex
		*/
		static uvec4 CastFloatColor(const vec4& colorEdge00, const vec4& colorEdge01, const vec4& colorEdge10, const vec4& colorEdge11);
	};
}