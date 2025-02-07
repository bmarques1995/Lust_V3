#include "PNGImage.hpp"
#include "FileHandler.hpp"
#include <cassert>
#include <fstream>

Lust::PNGImage::PNGImage(std::string_view path)
{
	LoadPNGFromFile(path);
	PostLoadAssert();
}

Lust::PNGImage::PNGImage(const std::byte* buffer, size_t size)
{
	LoadPNGFromMemory(buffer, size);
	PostLoadAssert();
}

Lust::PNGImage::PNGImage(const std::byte* rawBuffer, uint32_t width, uint32_t height)
{
	m_Width = width;
	m_Height = height;
	m_Channels = 4;
	m_Data = new uint8_t[4 * width * height];
	memcpy(m_Data, rawBuffer, (4 * width * height));
}

Lust::PNGImage::~PNGImage()
{
	delete[] m_Data;
}

void Lust::PNGImage::LoadPNGFromFile(std::string_view path)
{
	std::byte* buffer;
	size_t bufferSize;
	FileHandler::ReadBinFile(path, &buffer, &bufferSize);

	LoadPNGFromMemory(buffer, bufferSize);

	delete[] buffer;
}

void Lust::PNGImage::LoadPNGFromMemory(const std::byte* buffer, size_t size)
{
	png_structp pngPtr;
	png_infop infoPtr;

	InitPNGHandlers(&pngPtr, &infoPtr);
	ValidatePNGHeaders(buffer);
	size_t offset = 8;

	auto addr = (const std::byte*)(buffer + offset);

	png_set_read_fn(pngPtr, reinterpret_cast<png_voidp>(&addr), [](png_structp png_ptr, png_bytep data, png_size_t length) {
		const std::byte** buffer = (reinterpret_cast<const std::byte**>(png_get_io_ptr(png_ptr)));
		memcpy(data, (const void*)(*buffer), length);
		*buffer = (*buffer + length);
		});

	GetPNGData(&pngPtr, &infoPtr);
	SetPNGMembers(&pngPtr, &infoPtr);
	ExpandPNGToRGBA(&pngPtr, &infoPtr, png_get_color_type(pngPtr, infoPtr));
	LoadPNGImage(&pngPtr);
	EndPNGHandlers(&pngPtr, &infoPtr);
}

void Lust::PNGImage::InitPNGHandlers(png_structp* pngPtr2, png_infop* infoPtr2)
{
	*pngPtr2 = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	assert(pngPtr2);

	*infoPtr2 = png_create_info_struct(*pngPtr2);
	assert(infoPtr2);
}

void Lust::PNGImage::EndPNGHandlers(png_structp* pngPtr2, png_infop* infoPtr2)
{
	png_destroy_read_struct(pngPtr2, infoPtr2, nullptr);
}

void Lust::PNGImage::ValidatePNGHeaders(std::ifstream* pngFile)
{
	png_byte header[8];
	pngFile->read(reinterpret_cast<char*>(header), 8);
	assert(!png_sig_cmp(header, 0, 8));
}

void Lust::PNGImage::ValidatePNGHeaders(const std::byte* buffer)
{
	png_byte header[8];
	memcpy(header, buffer, 8);
	assert(!png_sig_cmp(header, 0, 8));
}

void Lust::PNGImage::ExpandPNGToRGBA(png_structp* pngPtr2, png_infop* infoPtr2, int colorType)
{
	switch (colorType)
	{
	case PNG_COLOR_TYPE_RGB:
	{
		png_set_add_alpha(*pngPtr2, 0xFF, PNG_FILLER_AFTER);
		png_read_update_info(*pngPtr2, *infoPtr2);
		return;
	}
	case PNG_COLOR_TYPE_GRAY:
	{
		png_set_gray_to_rgb(*pngPtr2);
		png_set_add_alpha(*pngPtr2, 0xFF, PNG_FILLER_AFTER);
		png_read_update_info(*pngPtr2, *infoPtr2);
		return;
	}
	case PNG_COLOR_TYPE_RGBA:
		return;
	default:
		assert(false);
	}
}

void Lust::PNGImage::GetPNGData(png_structp* pngPtr2, png_infop* infoPtr2)
{
	png_set_sig_bytes(*pngPtr2, 8); // Skip the header bytes
	png_read_info(*pngPtr2, *infoPtr2);
}

void Lust::PNGImage::SetPNGMembers(png_structp* pngPtr2, png_infop* infoPtr2)
{
	m_Width = png_get_image_width(*pngPtr2, *infoPtr2);
	m_Height = png_get_image_height(*pngPtr2, *infoPtr2);
	m_Channels = 4;

	size_t dataSize = m_Width * m_Height * m_Channels;
	m_Data = new uint8_t[dataSize];
}

void Lust::PNGImage::LoadPNGImage(png_structp* pngPtr2)
{
	png_bytep* row_pointers = new png_bytep[m_Height];
	for (int i = 0; i < m_Height; ++i) {
		row_pointers[i] = &m_Data[i * m_Width * m_Channels];
	}
	png_read_image(*pngPtr2, row_pointers);

	delete[] row_pointers;
}
