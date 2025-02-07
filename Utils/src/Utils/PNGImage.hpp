#pragma once

#include "Image.hpp"
#include <png.h>

namespace Lust
{
	class LUST_UTILS_API PNGImage : public Image
	{
	public:
		PNGImage(std::string_view path);
		PNGImage(const std::byte* buffer, size_t size);
		PNGImage(const std::byte* rawBuffer, uint32_t width, uint32_t height);
		virtual ~PNGImage();

	private:

		void LoadPNGFromFile(std::string_view path);
		void LoadPNGFromMemory(const std::byte* buffer, size_t size);
		static void InitPNGHandlers(png_structp* pngPtr2, png_infop* infoPtr2);
		static void EndPNGHandlers(png_structp* pngPtr2, png_infop* infoPtr2);
		void ValidatePNGHeaders(std::ifstream* pngFile);
		void ValidatePNGHeaders(const std::byte* buffer);
		void ExpandPNGToRGBA(png_structp* pngPtr2, png_infop* infoPtr2, int colorType);
		void GetPNGData(png_structp* pngPtr2, png_infop* infoPtr2);
		void SetPNGMembers(png_structp* pngPtr2, png_infop* infoPtr2);
		void LoadPNGImage(png_structp* pngPtr2);
	};
}
