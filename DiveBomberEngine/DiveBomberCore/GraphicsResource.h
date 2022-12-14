#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <chrono>

// D3D12 extension library.
#include "d3dx12.h"

#include "WRL.h"
#include "SystemConfig.h"
#include "GraphicsException.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"D3DCompiler.lib")
// #pragma comment(dll,"d3dcompiler_47.dll")
//const auto d3dCompiler = LoadLibraryEx("d3dcompiler_47.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

// The min/max macros conflict with like-named member functions.
// Only use std::min and std::max defined in <algorithm>.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif