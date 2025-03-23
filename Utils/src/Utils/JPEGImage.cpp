#include "JPEGImage.hpp"
#include <cassert>
#include <cstdio>

Lust::JPEGImage::JPEGImage(std::string_view path)
{
	LoadJPEGFromFile(path);
	PostLoadAlign();
}

Lust::JPEGImage::JPEGImage(const std::byte* data, size_t size)
{
	LoadJPEGFromMemory(data, size);
	PostLoadAlign();
}

Lust::JPEGImage::JPEGImage(const std::byte* rawBuffer, uint32_t width, uint32_t height)
{
	m_Width = width;
	m_Height = height;
	m_Channels = 4;
	m_Data = new uint8_t[4 * width * height];
	memcpy(m_Data, rawBuffer, (4 * width * height));
}

Lust::JPEGImage::~JPEGImage()
{
	delete[] m_Data;
}

void Lust::JPEGImage::LoadJPEGFromFile(std::string_view path)
{
	FILE* file;
#ifdef LUST_UTILS_WINDOWS
	fopen_s(&file, path.data(), "rb");
#else
	file = fopen(path.data(), "rb");
#endif
	assert(file);

	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	LoadJPEGPointers(&cinfo, &jerr, &file);
	DecompressJPEGImage(&cinfo);
	UnloadJPEGPointers(&cinfo, &jerr);
	fclose(file);
}

void Lust::JPEGImage::LoadJPEGFromMemory(const std::byte* data, size_t size)
{
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	LoadJPEGPointers(&cinfo, &jerr, data, size);
	DecompressJPEGImage(&cinfo);
	UnloadJPEGPointers(&cinfo, &jerr);
}

void Lust::JPEGImage::LoadJPEGPointers(jpeg_decompress_struct* cInfo, jpeg_error_mgr* jErr, FILE** content)
{
	cInfo->err = jpeg_std_error(jErr);
	jpeg_create_decompress(cInfo);

	jpeg_stdio_src(cInfo, *content);
	//jpeg_mem_src();
	jpeg_read_header(cInfo, TRUE);

	cInfo->out_color_space = JCS_EXT_RGBA; // Output 4 channels (RGBA) image
}

void Lust::JPEGImage::LoadJPEGPointers(jpeg_decompress_struct* cInfo, jpeg_error_mgr* jErr, const std::byte* data, size_t size)
{
	cInfo->err = jpeg_std_error(jErr);
	jpeg_create_decompress(cInfo);

	jpeg_mem_src(cInfo, reinterpret_cast<const unsigned char*>(data), (unsigned long)size);
	jpeg_read_header(cInfo, TRUE);

	cInfo->out_color_space = JCS_EXT_RGBA; // Output 4 channels (RGBA) image
}

void Lust::JPEGImage::DecompressJPEGImage(jpeg_decompress_struct* cInfo)
{
	jpeg_start_decompress(cInfo);

	m_Width = cInfo->output_width;
	m_Height = cInfo->output_height;
	m_Channels = 4;

	// Allocate memory for image data
	m_Data = new uint8_t[m_Width * m_Height * cInfo->output_components];

	JSAMPROW row_pointer[1];
	while (cInfo->output_scanline < cInfo->output_height) {
		row_pointer[0] = m_Data + (cInfo->output_scanline) * m_Width * cInfo->output_components;
		jpeg_read_scanlines(cInfo, row_pointer, 1);
	}
}

void Lust::JPEGImage::UnloadJPEGPointers(jpeg_decompress_struct* cInfo, jpeg_error_mgr* jErr)
{
	jpeg_finish_decompress(cInfo);
	jpeg_destroy_decompress(cInfo);
}
