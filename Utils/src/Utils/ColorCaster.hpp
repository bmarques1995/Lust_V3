#pragma once

#include "LustUtilsDLLMacro.hpp"
#include <Eigen/Eigen>

namespace Lust
{
	class LUST_UTILS_API ColorCaster
	{
	public:
		/**
			* @brief casts a float color to a uint32_t color, the color is in the format 0xRRGGBBAA, will be replicated to all vertices
			* @param color The color to cast
			*/
		static Eigen::Vector4<uint32_t> CastFloatColor(const Eigen::Vector3f& color);

		/**
		* @brief casts a float color to a uint32_t color, the color is in the format 0xRRGGBBAA, will be replicated to all vertices
		* @param color The color to cast
		*/
		static Eigen::Vector4<uint32_t> CastFloatColor(const Eigen::Vector4f& color);

		/**
		* @brief casts a float color to a uint32_t color, the color is in the format 0xRRGGBBAA, each vertex will have a different color
		* @param colorEdge00 The color of the first vertex
		* @param colorEdge01 The color of the second vertex
		* @param colorEdge10 The color of the third vertex
		* @param colorEdge11 The color of the fourth vertex
		*/
		static Eigen::Vector4<uint32_t> CastFloatColor(const Eigen::Vector3f& colorEdge00, const Eigen::Vector3f& colorEdge01, const Eigen::Vector3f& colorEdge10, const Eigen::Vector3f& colorEdge11);

		/**
		* @brief casts a float color to a uint32_t color, the color is in the format 0xRRGGBBAA, each vertex will have a different color
		* @param colorEdge00 The color of the first vertex
		* @param colorEdge01 The color of the second vertex
		* @param colorEdge10 The color of the third vertex
		* @param colorEdge11 The color of the fourth vertex
		*/
		static Eigen::Vector4<uint32_t> CastFloatColor(const Eigen::Vector4f& colorEdge00, const Eigen::Vector4f& colorEdge01, const Eigen::Vector4f& colorEdge10, const Eigen::Vector4f& colorEdge11);
	};
}