#pragma once

#ifdef LUST_UTILS_WINDOWS

#include <type_traits>
#include <cstdint>
#include <windows.h>
#include "Console.hpp"

namespace Lust
{
	/**
	* @brief A smart pointer class, used for COM objects, replacing COM smart pointers or ComPtr
	*/
	template<typename CT>
	class ComPointer
	{
	public:
		/**
		* @brief Default constructor
		*/
		ComPointer() = default;

		/**
		* @brief Construct by raw pointer (add ref)
		* @param pointer The raw pointer
		*/
		ComPointer(CT* pointer)
		{
			SetPointerAndAddRef(pointer);
		}

		/**
		* @brief Copy constructor
		* @param other The other ComPointer
		*/
		ComPointer(const ComPointer<CT>& other)
		{
			SetPointerAndAddRef(other.m_Pointer);
		}

		/**
		* @brief Move constructor, the other ComPointer is moved to the new ComPointer
		*/
		ComPointer(ComPointer<CT>&& other) noexcept
		{
			m_Pointer = other.m_Pointer;
			other.m_Pointer = nullptr;
		}

		/**
		* @brief Destructor
		*/
		~ComPointer()
		{
			ClearPointer();
		}

		/**
		* @brief Assignment operator
		*/
		ComPointer<CT>& operator=(const ComPointer<CT>& other)
		{
			ClearPointer();
			SetPointerAndAddRef(other.m_Pointer);
			return *this;
		}

		/**
		* @brief Move assignment
		*/
		ComPointer<CT>& operator=(ComPointer<CT>&& other)
		{
			ClearPointer();

			m_Pointer = other.m_Pointer;
			other.m_Pointer = nullptr;
			return *this;
		}

		/**
		* @brief Assignment operator
		*/
		ComPointer<CT>& operator=(CT* other)
		{
			ClearPointer();
			SetPointerAndAddRef(other);
			return *this;
		}

		/**
		* @brief Release the pointer and return the raw pointer, in practice is a destructor
		* @return The raw pointer
		*/
		uint32_t Release()
		{
			return ClearPointer();
		}

		/**
		* @brief Get the raw pointer and add ref
		* @return The raw pointer
		*/
		CT* GetRef()
		{
			if (m_Pointer)
			{
				m_Pointer->AddRef();
				return m_Pointer;
			}
			return nullptr;
		}

		/**
		* @brief Get the raw pointer and don't add ref
		* @return The raw pointer
		*/
		CT* GetConstRef() const
		{
			return m_Pointer;
		}

		/**
		* @brief Get the raw pointer
		* @return The raw pointer
		*/
		CT* Get()
		{
			return m_Pointer;
		}
		
		/**
		* @brief Get the raw pointer, in const format
		* @return The const raw pointer
		*/
		CT* GetConst() const
		{
			return m_Pointer;
		}

		/**
		* @brief Get the address of the raw pointer
		* @return The address of the raw pointer
		*/
		CT** GetAddressOf()
		{
			return &m_Pointer;
		}

		/**
		* @brief Check if the raw pointer is equal to the other raw pointer
		*/
		bool operator==(const ComPointer<CT>& other)
		{
			return m_Pointer == other.m_Pointer;
		}

		/**
		* @brief Check if the raw pointer is equal to the other raw pointer
		*/
		bool operator==(const CT* other)
		{
			return m_Pointer == other;
		}

		/**
		* @brief Get the raw pointer
		* @return The raw pointer
		*/
		CT* operator->()
		{
			return m_Pointer;
		}

		/**
		* @brief Get the raw pointer
		* @return The raw pointer
		*/
		CT** operator&()
		{
			return &m_Pointer;
		}

		/**
		* @brief Check if the raw pointer is not null
		* @return True if the raw pointer is not null
		*/
		operator bool()
		{
			return m_Pointer != nullptr;
		}

		/**
		* @brief Get the raw pointer
		* @return The raw pointer
		*/
		operator CT* ()
		{
			return m_Pointer;
		}

	private:

		/**
		* @brief Clear the raw pointer and return the ref count
		* @return The ref count
		*/
		uint32_t ClearPointer()
		{
			uint32_t newRef = 0;
			if (m_Pointer)
			{
				
				__try
				{
					newRef = m_Pointer->Release();
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					Console::CoreLog("Brute force exception");
					exit(6);
				}
				m_Pointer = nullptr;
			}

			return newRef;
		}

		/**
		* @brief Set the raw pointer and add ref
		* @param pointer The raw pointer
		*/
		void SetPointerAndAddRef(CT* pointer)
		{
			m_Pointer = pointer;
			if (m_Pointer)
			{
				m_Pointer->AddRef();
			}
		}

	private:
		CT* m_Pointer = nullptr;
	};
}

#endif