#ifdef LUST_UTILS_WINDOWS

#include <d3d12.h>
#include <gtest/gtest.h>
#include "ComPointer.hpp"

namespace Lust
{
	TEST(Utils, IsComPointerWorking)
	{
		ComPointer<ID3D12Device> pUnknown;
		EXPECT_EQ(pUnknown.Get(), nullptr);
		D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(pUnknown.GetAddressOf()));
		EXPECT_NE(pUnknown.Get(), nullptr);
		pUnknown.Release();
		EXPECT_EQ(pUnknown.Get(), nullptr);
	}

	TEST(Utils, IsComPointerOperatorsWorking)
	{
		ComPointer<ID3D12Device> pUnknown;
		D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(pUnknown.GetAddressOf()));
		EXPECT_NE(pUnknown.GetConst(), nullptr);
		EXPECT_TRUE(bool(pUnknown));
		EXPECT_EQ(&pUnknown, pUnknown.GetAddressOf());
		ComPointer<ID3D12Device> pUnknown2(pUnknown);
		EXPECT_EQ(pUnknown.GetConst(), pUnknown2.GetConst());
		ComPointer<ID3D12Device> pUnknown3(pUnknown.Get());
		EXPECT_EQ(pUnknown3.GetConst(), pUnknown2.GetConst());
		ComPointer<ID3D12Device> pUnknown4 = pUnknown;
		EXPECT_EQ(pUnknown4.GetConst(), pUnknown2.GetConst());
		ComPointer<ID3D12Device> pUnknown5 = pUnknown.Get();
		EXPECT_EQ(pUnknown5.GetConst(), pUnknown2.GetConst());
	}
}

#endif