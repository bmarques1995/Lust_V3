#pragma once

#include "Image.hpp"
#include <jpeglib.h>

namespace Lust
{
	/**
	* @brief A JPEG image
	* @details This class is used for JPEG images
	*/
	class LUST_UTILS_API JPEGImage : public Image
	{
	public:
		/**
		* @brief Default constructor
		* @param path The path to the JPEG image
		*/
		JPEGImage(std::string_view path);
		/**
		* @brief Constructor
		* @param data The data of the encoded JPEG image
		* @param size The size of the encoded JPEG image
		*/
		JPEGImage(const std::byte* data, size_t size);
		/**
		* @brief Constructor
		* @param rawBuffer The raw buffer of the decoded JPEG image
		* @param width The width of the decoded JPEG image
		* @param height The height of the decoded JPEG image
		*/
		JPEGImage(const std::byte* rawBuffer, uint32_t width, uint32_t height);
		/**
		* @brief Default destructor
		*/
		~JPEGImage();
	private:
		/**
		* @brief Loads a JPEG image from a file
		* @param path The path to the JPEG image
		*/
		void LoadJPEGFromFile(std::string_view path);
		/**
		* @brief Loads a JPEG image from memory
		* @param data The data of the encoded JPEG image
		* @param size The size of the encoded JPEG image
		*/
		void LoadJPEGFromMemory(const std::byte* data, size_t size);
		/**
		* @brief Loads a JPEG image from a file or memory, providing the necessary pointers
		* @param cInfo [Out] The JPEG decompression structure
		* @param jErr [Out] The JPEG error manager
		* @param content The content of the JPEG image
		*/
		void LoadJPEGPointers(jpeg_decompress_struct* cInfo, jpeg_error_mgr* jErr, FILE** content);
		/**
		* @brief Loads a JPEG image from memory, providing the necessary pointers
		* @param cInfo [Out] The JPEG decompression structure
		* @param jErr [Out] The JPEG error manager
		* @param data The data of the encoded JPEG image
		* @param size The size of the encoded JPEG image
		*/
		void LoadJPEGPointers(jpeg_decompress_struct* cInfo, jpeg_error_mgr* jErr, const std::byte* data, size_t size);
		/**
		* @brief Decompresses a JPEG image
		* @param cInfo The JPEG decompression structure
		*/
		void DecompressJPEGImage(jpeg_decompress_struct* cInfo);
		/**
		* @brief Unloads the JPEG pointers
		* @param cInfo The JPEG decompression structure
		* @param jErr The JPEG error manager
		*/
		void UnloadJPEGPointers(jpeg_decompress_struct* cInfo, jpeg_error_mgr* jErr);
	};
}
