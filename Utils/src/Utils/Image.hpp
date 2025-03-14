#pragma once

#include "LustUtilsDLLMacro.hpp"
#include <memory>
#include <cstdint>
#include <string>
#include <png.h>
#include <jpeglib.h>
#include <unordered_map>

namespace Lust
{
	enum class ImageFormat
	{
		UNKNOWN = 0,
		PNG = 1,
		JPEG = 2
	};

	enum class ImageAlignment
	{
		PERFECT = 0,
		COMPENSED = 1
	};

	class LUST_UTILS_API Image
	{
	public:
		virtual ~Image() = default;

		virtual unsigned char* GetRawPointer() const;
		virtual uint32_t GetWidth() const;
		virtual uint32_t GetHeight() const;
		virtual uint32_t GetChannels() const;
		virtual uint32_t GetMips() const;
		virtual ImageAlignment GetImageAlignment() const;

		static Image* CreateImage(std::string_view path);
		static Image* CreateImage(const std::byte* buffer, size_t dataSize, ImageFormat format);
		static Image* CreateImage(const std::byte* buffer, uint32_t width, uint32_t height, ImageFormat format);

		static void CastBMPToPNG(const unsigned char* pixels, uint32_t width, uint32_t height, unsigned char** buffer, size_t* bufferSize, bool flipVertically = false);
		static void CastBMPToJPEG(const unsigned char* pixels, uint32_t width, uint32_t height, unsigned char** buffer, size_t* bufferSize, uint32_t quality, bool flipVertically = false);

	protected:
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Channels;
		ImageAlignment m_ImageAlignment = ImageAlignment::COMPENSED;

		void PostLoadAlign();

		unsigned char* m_Data;

	private:
		static ImageFormat GetImageFormat(std::string_view path);

		static const std::unordered_map<std::string_view, ImageFormat> s_ValidFormats;
	};

	/*
	class LUST_API GenImage : public Image
	{
	public:
		GenImage(std::string_view path);
		~GenImage();

		unsigned char* GetRawPointer() override;

		uint32_t GetWidth() override;
		uint32_t GetHeight() override;
		uint32_t GetChannels() override;

		uint32_t GetMips() override;

	private:
		void SetImageFormat(std::string_view path);
		void LoadPNG(std::string_view path);
		static void InitPNGHandlers(png_structp* pngPtr2, png_infop* infoPtr2);
		static void EndPNGHandlers(png_structp* pngPtr2, png_infop* infoPtr2);
		void ValidatePNGHeaders(std::ifstream* pngFile);
		void ValidatePNGHeaders(std::byte* buffer);
		void ExpandPNGToRGBA(png_structp* pngPtr2, png_infop* infoPtr2, int colorType);
		void GetPNGData(png_structp* pngPtr2, png_infop* infoPtr2);
		void SetPNGMembers(png_structp* pngPtr2, png_infop* infoPtr2);
		void LoadPNGImage(png_structp* pngPtr2);
		void LoadJPEG(std::string_view path);
		void LoadJPEGPointers(jpeg_decompress_struct* cInfo, jpeg_error_mgr* jErr, FILE** content);
		void DecompressJPEGImage(jpeg_decompress_struct* cInfo);
		void UnloadJPEGPointers(jpeg_decompress_struct* cInfo, jpeg_error_mgr* jErr);
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Channels;
		ImageFormat m_Extension;

		unsigned char* m_Data;

		static const std::unordered_map<std::string_view, ImageFormat> s_ValidFormats;

		//static png_structp s_PngPtr;
		//static png_infop s_InfoPtr;

		//static jpeg_decompress_struct s_CInfo;
		//static jpeg_error_mgr s_JErr;
	};*/
}
