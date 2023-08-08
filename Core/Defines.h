#pragma once
#ifndef MY_DEFINES
#define MY_DEFINES

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef  WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <d3d12.h>

#include <cassert>

#ifdef ASSERT
#undef ASSERT
#endif 


#define ASSERT(x) assert(x)
#define ASSERT_SUCCEEDED(hr) ASSERT(SUCCEEDED(hr))

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define WSTRINGIFY_(x) L##x
#define WSTRINGIFY(x) WSTRINGIFY_(x)

#ifdef _DEBUG
#define NAME_D3D12_OBJECT(x) x->SetName(WSTRINGIFY(__FILE__ "(" STRINGIFY(__LINE__) "): " Lx) )
#else
#define NAME_D3D12_OBJECT(x) (void)(x)
#endif // _DEBUG

#ifndef D3D12_GPU_VIRTUAL_ADDRESS_NULL
#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#endif

#define UNIT_KB(x) (x * 1024)
#define UNIT_MB(x) (x * 1024 * 1024)

#endif // MY_DEFINES