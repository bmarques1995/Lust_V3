#include "ColorCaster.hpp"

Eigen::Vector4<uint32_t> Lust::ColorCaster::CastFloatColor(const Eigen::Vector3f& color)
{
	uint8_t r = (uint8_t)(color(0) * 255.0f);
	uint8_t g = (uint8_t)(color(1) * 255.0f);
	uint8_t b = (uint8_t)(color(2) * 255.0f);
	uint32_t pixel = (r << 24) | (g << 16) | (b << 8) | 0xff;
	return Eigen::Vector4<uint32_t>(pixel, pixel, pixel, pixel);
}

Eigen::Vector4<uint32_t> Lust::ColorCaster::CastFloatColor(const Eigen::Vector4f& color)
{
	uint8_t r = (uint8_t)(color(0) * 255.0f);
	uint8_t g = (uint8_t)(color(1) * 255.0f);
	uint8_t b = (uint8_t)(color(2) * 255.0f);
	uint8_t a = (uint8_t)(color(3) * 255.0f);
	uint32_t pixel = (r << 24) | (g << 16) | (b << 8) | a;
	return Eigen::Vector4<uint32_t>(pixel, pixel, pixel, pixel);
}

Eigen::Vector4<uint32_t> Lust::ColorCaster::CastFloatColor(const Eigen::Vector3f& colorEdge00, const Eigen::Vector3f& colorEdge01, const Eigen::Vector3f& colorEdge10, const Eigen::Vector3f& colorEdge11)
{
	uint8_t r00 = (uint8_t)(colorEdge00(0) * 255.0f);
	uint8_t g00 = (uint8_t)(colorEdge00(1) * 255.0f);
	uint8_t b00 = (uint8_t)(colorEdge00(2) * 255.0f);
	uint32_t pixel00 = (r00 << 24) | (g00 << 16) | (b00 << 8) | 0xff;
	uint8_t r01 = (uint8_t)(colorEdge01(0) * 255.0f);
	uint8_t g01 = (uint8_t)(colorEdge01(1) * 255.0f);
	uint8_t b01 = (uint8_t)(colorEdge01(2) * 255.0f);
	uint32_t pixel01 = (r01 << 24) | (g01 << 16) | (b01 << 8) | 0xff;
	uint8_t r10 = (uint8_t)(colorEdge10(0) * 255.0f);
	uint8_t g10 = (uint8_t)(colorEdge10(1) * 255.0f);
	uint8_t b10 = (uint8_t)(colorEdge10(2) * 255.0f);
	uint32_t pixel10 = (r10 << 24) | (g10 << 16) | (b10 << 8) | 0xff;
	uint8_t r11 = (uint8_t)(colorEdge11(0) * 255.0f);
	uint8_t g11 = (uint8_t)(colorEdge11(1) * 255.0f);
	uint8_t b11 = (uint8_t)(colorEdge11(2) * 255.0f);
	uint32_t pixel11 = (r11 << 24) | (g11 << 16) | (b11 << 8) | 0xff;
	return Eigen::Vector4<uint32_t>(pixel00, pixel01, pixel10, pixel11);
}

Eigen::Vector4<uint32_t> Lust::ColorCaster::CastFloatColor(const Eigen::Vector4f& colorEdge00, const Eigen::Vector4f& colorEdge01, const Eigen::Vector4f& colorEdge10, const Eigen::Vector4f& colorEdge11)
{
	uint8_t r00 = (uint8_t)(colorEdge00(0) * 255.0f);
	uint8_t g00 = (uint8_t)(colorEdge00(1) * 255.0f);
	uint8_t b00 = (uint8_t)(colorEdge00(2) * 255.0f);
	uint8_t a00 = (uint8_t)(colorEdge00(3) * 255.0f);
	uint32_t pixel00 = (r00 << 24) | (g00 << 16) | (b00 << 8) | a00;
	uint8_t r01 = (uint8_t)(colorEdge01(0) * 255.0f);
	uint8_t g01 = (uint8_t)(colorEdge01(1) * 255.0f);
	uint8_t b01 = (uint8_t)(colorEdge01(2) * 255.0f);
	uint8_t a01 = (uint8_t)(colorEdge01(3) * 255.0f);
	uint32_t pixel01 = (r01 << 24) | (g01 << 16) | (b01 << 8) | a01;
	uint8_t r10 = (uint8_t)(colorEdge10(0) * 255.0f);
	uint8_t g10 = (uint8_t)(colorEdge10(1) * 255.0f);
	uint8_t b10 = (uint8_t)(colorEdge10(2) * 255.0f);
	uint8_t a10 = (uint8_t)(colorEdge10(3) * 255.0f);
	uint32_t pixel10 = (r10 << 24) | (g10 << 16) | (b10 << 8) | a10;
	uint8_t r11 = (uint8_t)(colorEdge11(0) * 255.0f);
	uint8_t g11 = (uint8_t)(colorEdge11(1) * 255.0f);
	uint8_t b11 = (uint8_t)(colorEdge11(2) * 255.0f);
	uint8_t a11 = (uint8_t)(colorEdge11(3) * 255.0f);
	uint32_t pixel11 = (r11 << 24) | (g11 << 16) | (b11 << 8) | a11;
	return Eigen::Vector4<uint32_t>(pixel00, pixel01, pixel10, pixel11);
}
