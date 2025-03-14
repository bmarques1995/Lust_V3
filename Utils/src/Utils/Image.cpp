#include "Image.hpp"

#include <regex>
#include "PNGImage.hpp"
#include "JPEGImage.hpp"
#include <zlib.h>
#include <cassert>

const std::unordered_map<std::string_view, Lust::ImageFormat> Lust::Image::s_ValidFormats =
{
	{"png", ImageFormat::PNG},
	{"jpg", ImageFormat::JPEG},
	{"jpeg", ImageFormat::JPEG}
};

unsigned char* Lust::Image::GetRawPointer() const
{
	return m_Data;
}

uint32_t Lust::Image::GetWidth() const
{
	return m_Width;
}

uint32_t Lust::Image::GetHeight() const
{
	return m_Height;
}

uint32_t Lust::Image::GetChannels() const
{
	return m_Channels;
}

uint32_t Lust::Image::GetMips() const
{
	uint32_t mipLevels = 1;

	uint32_t comparator = std::max(m_Width, m_Height);

	while (comparator > 1)
	{
		comparator = std::max(comparator / 2, (uint32_t)1);
		++mipLevels;
	}

	return mipLevels;
}

Lust::ImageAlignment Lust::Image::GetImageAlignment() const
{
	return m_ImageAlignment;
}

Lust::Image* Lust::Image::CreateImage(std::string_view path)
{
	ImageFormat format = GetImageFormat(path);
	switch (format)
	{
	case Lust::ImageFormat::PNG: return new PNGImage(path);
	case Lust::ImageFormat::JPEG: return new JPEGImage(path);
	default: return nullptr;
	}
}

Lust::Image* Lust::Image::CreateImage(const std::byte* buffer, size_t dataSize, ImageFormat format)
{
	switch (format)
	{
	case Lust::ImageFormat::PNG: return new PNGImage(buffer, dataSize);
	case Lust::ImageFormat::JPEG: return new JPEGImage(buffer, dataSize);
	default: return nullptr;
	}
}

Lust::Image* Lust::Image::CreateImage(const std::byte* buffer, uint32_t width, uint32_t height, ImageFormat format)
{
	switch (format)
	{
	case Lust::ImageFormat::PNG: return new PNGImage(buffer, width, height);
	case Lust::ImageFormat::JPEG: return new JPEGImage(buffer, width, height);
	default: return nullptr;
	}
}

Lust::ImageFormat Lust::Image::GetImageFormat(std::string_view path)
{
	ImageFormat format;
	std::string filePath = path.data();
	std::regex extensionRegex(".*\\.([a-zA-Z0-9]+)$");
	std::smatch match;
	std::string extension;
	if (std::regex_search(filePath, match, extensionRegex)) {
		extension = match.str(1);
	}
	else
	{
		assert(false);
	}
	std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) {
		return std::tolower(c);
		});
	auto it = s_ValidFormats.find(extension);
	if (it == s_ValidFormats.end())
	{
		format = ImageFormat::UNKNOWN;
	}
	else
		format = it->second;
	return format;
}

void Lust::Image::CastBMPToPNG(const unsigned char* pixels, uint32_t width, uint32_t height, unsigned char** buffer, size_t* bufferSize, bool flipVertically)
{
	std::vector<unsigned char> out;
	out.clear();
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	assert(png);
	png_infop info = png_create_info_struct(png);
	assert(info);
	assert(0 == setjmp(png_jmpbuf(png)));
	png_set_IHDR(png, info, width, height, 8,
		PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);
	png_set_compression_level(png, Z_BEST_COMPRESSION);
	std::vector<unsigned char*> rows(height);
	for (size_t y = 0; y < height; ++y)
		if (flipVertically)
			rows[height - y - 1] = (unsigned char*)pixels + y * width * 4;
		else
			rows[y] = (unsigned char*)pixels + y * width * 4;
	png_set_rows(png, info, &rows[0]);
	png_set_write_fn(png, &out, [](png_structp  png_ptr, png_bytep data, png_size_t length)
		{
			std::vector<unsigned char>* p = (std::vector<unsigned char>*)png_get_io_ptr(png_ptr);
			p->insert(p->end(), data, data + length);
		}, NULL);
	png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
	png_destroy_write_struct(&png, &info);
	*bufferSize = out.size();
	*buffer = new unsigned char[*bufferSize];
	memcpy(*buffer, out.data(), *bufferSize);
}

void Lust::Image::CastBMPToJPEG(const unsigned char* pixels, uint32_t width, uint32_t height, unsigned char** buffer, size_t* bufferSize, uint32_t quality, bool flipVertically)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	// Initialize the JPEG compression object
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	// Create an in-memory destination manager
	uint8_t* memBuffer = nullptr;
	unsigned long memSize = 0;
	jpeg_mem_dest(&cinfo, &memBuffer, &memSize);

	// Set image parameters
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;  // RGB components
	cinfo.in_color_space = JCS_RGB;

	// Set compression options
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);

	// Start compression
	jpeg_start_compress(&cinfo, TRUE);

	// Write scanlines
	JSAMPROW row_pointer[1];
	int row_stride = width * cinfo.input_components;

	uint8_t* rgbBuffer = new uint8_t[width * height * 3];

	// Convert RGBA to RGB
	for (uint32_t i = 0; i < width * height; ++i) {
		rgbBuffer[i * 3] = pixels[i * 4];         // Red
		rgbBuffer[i * 3 + 1] = pixels[i * 4 + 1]; // Green
		rgbBuffer[i * 3 + 2] = pixels[i * 4 + 2]; // Blue
	}

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = (JSAMPROW)&rgbBuffer[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	// Finish compression
	jpeg_finish_compress(&cinfo);

	// Get the compressed data from the memory buffer
	*bufferSize = static_cast<size_t>(memSize);
	*buffer = new uint8_t[*bufferSize];
	std::memcpy(*buffer, memBuffer, *bufferSize);

	// Clean up
	delete[] memBuffer;
	delete[] rgbBuffer;
	jpeg_destroy_compress(&cinfo);
}

void Lust::Image::PostLoadAlign()
{
	assert(m_Channels == 4);
	m_ImageAlignment = ((m_Width % 64) == 0) ? ImageAlignment::PERFECT : ImageAlignment::COMPENSED;
}
