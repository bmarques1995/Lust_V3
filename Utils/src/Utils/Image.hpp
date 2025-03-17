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
	/**
	 * @enum ImageFormat
	 * @brief Enumerates the supported image formats
	 */
	enum class ImageFormat
	{
		UNKNOWN = 0,
		PNG = 1,
		JPEG = 2
	};

	/**
	 * @enum ImageAlignment
	 * @brief Enumerates the image alignment options
	 * @param PERFECT The perfect alignment means that the image width is a multiple of the greater Vulkan's required alignment or Direct3D's (12) required alignment divided by 4
	 * @param COMPENSED The compensated alignment means that the image alignment is not perfect
	 */
	enum class ImageAlignment
	{
		PERFECT = 0,
		COMPENSED = 1
	};

	/**
	* @brief Abstract class representing an image
	*/
	class LUST_UTILS_API Image
	{
	public:
		virtual ~Image() = default;

		/**
		* @brief Returns a pointer to the raw image data
		* @return A pointer to the raw image data
		*/
		virtual unsigned char* GetRawPointer() const;
		
		/**
		* @brief Returns the width of the image
		* @return The width of the image
		*/
		virtual uint32_t GetWidth() const;

		/**
		* @brief Returns the height of the image
		* @return The height of the image
		*/
		virtual uint32_t GetHeight() const;
		
		/**
		* @brief Returns the number of channels of the image
		* @return The number of channels of the image
		* @note The number of channels could be 1(R), 2(RG), 3(RGB) or 4(RGBA), for this program, is enforced to be 4
		*/
		virtual uint32_t GetChannels() const;

		/**
		* @brief Returns the number of mipmap levels of the image
		* @return The number of mipmap levels of the image
		* @details The number of mipmap levels is the log_2 of the greatest dimension
		*/
		virtual uint32_t GetMips() const;

		/**
		* @brief Returns the alignment of the image
		*/
		virtual ImageAlignment GetImageAlignment() const;

		/**
		* @brief Creates an image from a file
		* @note The file must be a supported image, and the function analyze the file extension to determine the image format
		*/
		static Image* CreateImage(std::string_view path);
		
		/**
		* @brief Creates an image from a buffer, in the file format specified by the format parameter
		* @param buffer The buffer containing the image data
		* @param dataSize The size of the buffer in bytes
		* @param format The format of the image
		* @note If the buffer is not encoded in a supported image format, the function will fail
		*/
		static Image* CreateImage(const std::byte* buffer, size_t dataSize, ImageFormat format);
		
		/**
		* @brief Creates an image from a buffer, in the file format specified by the format parameter
		* @param buffer The buffer containing the pixels
		* @param width The width of the image
		* @param height The height of the image
		* @param format The format of the image
		* @note The image format is only for output
		*/
		static Image* CreateImage(const std::byte* buffer, uint32_t width, uint32_t height, ImageFormat format);

		/**
		* @brief Casts a BMP image to a PNG or JPEG image
		* @param pixels The pixels of the BMP image
		* @param width The width of the BMP image
		* @param height The height of the BMP image
		* @param buffer [Out] The buffer to store the casted image
		* @param bufferSize [Out] The size of the buffer in bytes
		* @param flipVertically If true, the image will be flipped vertically
		*/
		static void CastBMPToPNG(const unsigned char* pixels, uint32_t width, uint32_t height, unsigned char** buffer, size_t* bufferSize, bool flipVertically = false);
		
		/**
		* @brief Casts a BMP image to a JPEG image
		* @param pixels The pixels of the BMP image
		* @param width The width of the BMP image
		* @param height The height of the BMP image
		* @param buffer [Out] The buffer to store the casted image
		* @param bufferSize [Out] The size of the buffer in bytes
		* @param quality The quality of the JPEG image
		* @param flipVertically If true, the image will be flipped vertically
		*/
		static void CastBMPToJPEG(const unsigned char* pixels, uint32_t width, uint32_t height, unsigned char** buffer, size_t* bufferSize, uint32_t quality, bool flipVertically = false);

	protected:
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Channels;
		ImageAlignment m_ImageAlignment = ImageAlignment::COMPENSED;

		/**
		* @brief Asserts that the image is aligned, and its channels are 4
		* @note This function should be called after the image is loaded, and will determine the image alignment
		*/
		void PostLoadAlign();

		unsigned char* m_Data;

	private:
		/**
		* @brief Get the image format of a file
		* @param path The path of the file
		* @return The image format
		*/
		static ImageFormat GetImageFormat(std::string_view path);

		static const std::unordered_map<std::string_view, ImageFormat> s_ValidFormats;
	};
}
