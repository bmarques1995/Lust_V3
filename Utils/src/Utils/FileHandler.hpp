#pragma once

#include "LustUtilsDLLMacro.hpp"
#include <string>
#include <utility>

namespace Lust
{
	/**
	* @brief A class for handling files
	*/
	class LUST_UTILS_API FileHandler
	{
	public:
		/**
		* @brief Read a text file
		* @param path The path of the file
		* @param content [Out] The content of the file
		*/
		static bool ReadTextFile(std::string_view path, std::string* content);
		/**
		* @brief Write a text file
		* @param path The path of the file
		* @param content The content of the file
		*/
		static bool WriteTextFile(std::string_view path, std::string content);
		/**
		* @brief Read a binary file
		* @param path The path of the file
		* @param content [Out] The content of the file
		* @param fileSize [Out] The size of the file
		*/
		static bool ReadBinFile(std::string_view path, std::byte** content, size_t* fileSize);
		/**
		* @brief Write a binary file
		* @param path The path of the file
		* @param content The content of the file
		*/
		static bool WriteBinFile(std::string_view path, std::byte* content, size_t dataSize);
		/**
		* @brief Check if a file exists
		* @param path The path of the file
		*/
		static bool FileExists(std::string_view path);
	};
}