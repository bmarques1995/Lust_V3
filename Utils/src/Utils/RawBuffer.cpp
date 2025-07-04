#include "RawBuffer.hpp"
#include <cstring>
#include <zstd.h>
#include <openssl/evp.h>
#include <cstdlib>
#include "Console.hpp"

const uint8_t Lust::RawBuffer::s_AESKey[32] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f

};
const uint8_t Lust::RawBuffer::s_AESIv[16] = 
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

Lust::RawBuffer::RawBuffer()
{
	m_Data = nullptr;
	m_Size = 0;
}

Lust::RawBuffer::RawBuffer(uint8_t* data, size_t size)
{
	m_Size = size;
	m_Data = new uint8_t[m_Size];
	memcpy(m_Data, data, m_Size);
}

Lust::RawBuffer::~RawBuffer()
{
	if (m_Data != nullptr)
		delete[] m_Data;
}

void Lust::RawBuffer::RecreateBuffer(uint8_t* data, size_t size)
{
	if (m_Data != nullptr)
		delete[] m_Data;

	m_Size = size;
	m_Data = new uint8_t[m_Size];
	memcpy(m_Data, data, m_Size);
}

void Lust::RawBuffer::DeflateBuffer()
{
	if (m_Data == nullptr)
		return;
	size_t max_size = ZSTD_compressBound(m_Size);
	uint8_t* tempData = new uint8_t[max_size];
	size_t tempSize = ZSTD_compress(tempData, max_size, m_Data, m_Size, 9);
	RecreateBuffer(tempData, tempSize);
	delete[] tempData;

}

void Lust::RawBuffer::InflateBuffer()
{
	if (m_Data == nullptr)
		return;
	size_t max_size = ZSTD_getFrameContentSize(m_Data, m_Size);
	Console::CoreAssert(max_size != ZSTD_CONTENTSIZE_ERROR, "Failed to decompress buffer");
	if (max_size == 0)
		return;
	uint8_t* tempData = new uint8_t[max_size];
	size_t tempSize = ZSTD_decompress(tempData, max_size, m_Data, m_Size);
	RecreateBuffer(tempData, tempSize);
	delete[] tempData;
}

void Lust::RawBuffer::CypherBuffer()
{
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
		return;

	if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, s_AESKey, s_AESIv) != 1)
	{
		EVP_CIPHER_CTX_free(ctx);
		return;
	}

	size_t max_size = m_Size + EVP_CIPHER_block_size(EVP_aes_256_cbc());
	uint8_t* tempData = new uint8_t[max_size];

	int out_len1 = 0;
	if (EVP_EncryptUpdate(ctx, tempData, &out_len1, m_Data, m_Size) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		delete[] tempData;
		return;
	}

	int out_len2 = 0;
	if (EVP_EncryptFinal_ex(ctx, tempData + out_len1, &out_len2) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		delete[] tempData;
		return;
	}

	size_t tempSize = out_len1 + out_len2;
	EVP_CIPHER_CTX_free(ctx);
	RecreateBuffer(tempData, tempSize);
	delete[] tempData;
}

void Lust::RawBuffer::DecypherBuffer()
{
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
		return;

	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, s_AESKey, s_AESIv) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		return;
	}

	uint8_t* tempData = new uint8_t[m_Size];
	int out_len1 = 0;
	if (EVP_DecryptUpdate(ctx, tempData, &out_len1, m_Data, m_Size) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		delete[] tempData;
		return;
	}

	int out_len2 = 0;
	if (EVP_DecryptFinal_ex(ctx, tempData + out_len1, &out_len2) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		delete[] tempData;
		return;
	}

	size_t tempSize = out_len1 + out_len2;
	EVP_CIPHER_CTX_free(ctx);
	RecreateBuffer(tempData, tempSize);
	delete[] tempData;
}

const uint8_t* Lust::RawBuffer::GetData() const 
{
	return m_Data;
}

const size_t Lust::RawBuffer::GetSize() const
{
	return m_Size;
}
