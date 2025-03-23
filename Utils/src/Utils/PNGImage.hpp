#pragma once

#include "Image.hpp"
#include <png.h>

namespace Lust
{
	/**
	* @brief A PNG image
	*/
	class LUST_UTILS_API PNGImage : public Image
	{
	public:
		/**
		* @brief Default constructor
		* @param path The path to the PNG image
		*/
		PNGImage(std::string_view path);
		/**
		* @brief Constructor
		* @param buffer The buffer of the encoded PNG image
		* @param size The size of the encoded PNG image
		*/
		PNGImage(const std::byte* buffer, size_t size);
		/**
		* @brief Constructor
		* @param rawBuffer The raw buffer of the decoded PNG image
		* @param width The width of the decoded PNG image
		* @param height The height of the decoded PNG image
		*/
		PNGImage(const std::byte* rawBuffer, uint32_t width, uint32_t height);
		/**
		* @brief Destructor
		*/
		virtual ~PNGImage();

	private:
		/**
		* @brief Loads a PNG image from a file
		* @param path The path to the PNG image
		*/
		void LoadPNGFromFile(std::string_view path);
		/**
		* @brief Loads a PNG image from memory
		* @param buffer The buffer of the encoded PNG image
		* @param size The size of the encoded PNG image
		*/
		void LoadPNGFromMemory(const std::byte* buffer, size_t size);
		/**
		* @brief Initializes the PNG handlers
		* @param pngPtr2 The PNG pointer
		* @param infoPtr2 The PNG info pointer
		*/
		static void InitPNGHandlers(png_structp* pngPtr2, png_infop* infoPtr2);
		/**
		* @brief Ends the PNG handlers
		* @param pngPtr2 The PNG pointer
		* @param infoPtr2 The PNG info pointer
		*/
		static void EndPNGHandlers(png_structp* pngPtr2, png_infop* infoPtr2);
		/**
		* @brief Validates the PNG headers
		* @param buffer The buffer of the encoded PNG image
		*/
		void ValidatePNGHeaders(const std::byte* buffer);
		/**
		* @brief Expands the PNG image to RGBA
		* @param pngPtr2 The PNG pointer
		* @param infoPtr2 The PNG info pointer
		* @param colorType The color type, now it's handled only GRAY, RGB and RGBA(with this preferred)
		*/
		void ExpandPNGToRGBA(png_structp* pngPtr2, png_infop* infoPtr2, int colorType);
		/**
		* @brief Gets the PNG data
		* @param pngPtr2 The PNG pointer
		* @param infoPtr2 The PNG info pointer
		*/
		void GetPNGData(png_structp* pngPtr2, png_infop* infoPtr2);
		/**
		* @brief Sets the PNG members
		* @param pngPtr2 The PNG pointer
		* @param infoPtr2 The PNG info pointer
		*/
		void SetPNGMembers(png_structp* pngPtr2, png_infop* infoPtr2);
		/**
		* @brief Loads the PNG image
		* @param pngPtr2 The PNG pointer
		*/
		void LoadPNGImage(png_structp* pngPtr2);
	};
}
