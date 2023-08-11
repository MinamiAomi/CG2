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


#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define WSTRINGIFY_(x) L#x
#define WSTRINGIFY(x) WSTRINGIFY_(x)

void AssertIfFailed(HRESULT hr, const char* str, const char* file, const char* line);

#define ASSERT_IF_FAILED(hr) AssertIfFailed(hr, STRINGIFY(hr), __FILE__, STRINGIFY(__LINE__))

#ifdef _DEBUG
#define NAME_D3D12_OBJECT(x) x->SetName(WSTRINGIFY(__FILE__ "(" STRINGIFY(__LINE__) "): " WSTRINGIFY(x)))
#else
#define NAME_D3D12_OBJECT(x) ((void)0)
#endif // _DEBUG

#ifndef D3D12_GPU_VIRTUAL_ADDRESS_NULL
#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#endif

#ifndef D3D12_CPU_DESCRIPTOR_HANDLE_NULL
#define D3D12_CPU_DESCRIPTOR_HANDLE_NULL (D3D12_CPU_DESCRIPTOR_HANDLE{0})
#endif

#ifndef D3D12_GPU_DESCRIPTOR_HANDLE_NULL
#define D3D12_GPU_DESCRIPTOR_HANDLE_NULL (D3D12_GPU_DESCRIPTOR_HANDLE{0})
#endif

#define UNIT_KB(x) (x * 1024)
#define UNIT_MB(x) (x * 1024 * 1024)

#endif // MY_DEFINES