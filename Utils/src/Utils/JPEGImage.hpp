#pragma once

#include "Image.hpp"
#include <jpeglib.h>

namespace Lust
{
	class LUST_UTILS_API JPEGImage : public Image
	{
	public:
		JPEGImage(std::string_view path);
		JPEGImage(const std::byte* data, size_t size);
		JPEGImage(const std::byte* rawBuffer, uint32_t width, uint32_t height);
		~JPEGImage();
	private:
		void LoadJPEGFromFile(std::string_view path);
		void LoadJPEGFromMemory(const std::byte* data, size_t size);
		void LoadJPEGPointers(jpeg_decompress_struct* cInfo, jpeg_error_mgr* jErr, FILE** content);
		void LoadJPEGPointers(jpeg_decompress_struct* cInfo, jpeg_error_mgr* jErr, const std::byte* data, size_t size);
		void DecompressJPEGImage(jpeg_decompress_struct* cInfo);
		void UnloadJPEGPointers(jpeg_decompress_struct* cInfo, jpeg_error_mgr* jErr);
	};
}
