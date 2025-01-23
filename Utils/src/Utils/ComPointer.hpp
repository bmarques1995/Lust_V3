#pragma once

#include <type_traits>
#include <cstdint>

namespace Lust
{
	template<typename CT>
	class ComPointer
	{
	public:
		// Default empty constructor
		ComPointer() = default;

		// Construct by raw pointer (add ref)
		ComPointer(CT* pointer)
		{
			SetPointerAndAddRef(pointer);
		}

		ComPointer(const ComPointer<CT>& other)
		{
			SetPointerAndAddRef(other.m_Pointer);
		}
		ComPointer(ComPointer<CT>&& other) noexcept
		{
			m_Pointer = other.m_Pointer;
			other.m_Pointer = nullptr;
		}

		~ComPointer()
		{
			ClearPointer();
		}

		ComPointer<CT>& operator=(const ComPointer<CT>& other)
		{
			ClearPointer();
			SetPointerAndAddRef(other.m_Pointer);
			return *this;
		}

		ComPointer<CT>& operator=(ComPointer<CT>&& other)
		{
			ClearPointer();

			m_Pointer = other.m_Pointer;
			other.m_Pointer = nullptr;
			return *this;
		}

		ComPointer<CT>& operator=(CT* other)
		{
			ClearPointer();
			SetPointerAndAddRef(other);
			return *this;
		}

		uint32_t Release()
		{
			return ClearPointer();
		}

		CT* GetRef()
		{
			if (m_Pointer)
			{
				m_Pointer->AddRef();
				return m_Pointer;
			}
			return nullptr;
		}

		CT* GetConstRef() const
		{
			return GetRef();
		}

		CT* Get()
		{
			return m_Pointer;
		}

		CT* GetConst() const
		{
			return m_Pointer;
		}

		CT** GetAddressOf()
		{
			return &m_Pointer;
		}

		bool operator==(const ComPointer<CT>& other)
		{
			return m_Pointer == other.m_Pointer;
		}
		bool operator==(const CT* other)
		{
			return m_Pointer == other;
		}

		CT* operator->()
		{
			return m_Pointer;
		}
		CT** operator&()
		{
			return &m_Pointer;
		}

		operator bool()
		{
			return m_Pointer != nullptr;
		}
		operator CT* ()
		{
			return m_Pointer;
		}

	private:
		uint32_t ClearPointer()
		{
			uint32_t newRef = 0;
			if (m_Pointer)
			{
				newRef = m_Pointer->Release();
				m_Pointer = nullptr;
			}

			return newRef;
		}

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