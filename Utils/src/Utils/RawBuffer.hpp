#pragma once
#include <cstdint>
#include "LustUtilsDLLMacro.hpp"

namespace Lust
{
	class LUST_UTILS_API RawBuffer
	{
	public:
		RawBuffer();
		RawBuffer(uint8_t* data, size_t size);
		~RawBuffer();

		void RecreateBuffer(uint8_t* data, size_t size);

		void DeflateBuffer();
		void InflateBuffer();

		void CypherBuffer();
		void DecypherBuffer();

		const uint8_t* GetData() const;
		const size_t GetSize() const;

	private:
		static const uint8_t s_AESKey[32];
		static const uint8_t s_AESIv[16];

		uint8_t* m_Data;
		size_t m_Size;
	};
}