#pragma once
// D3D12 extension library.
#include "..\Exception\GraphicsException.h"

#include <d3dcompiler.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"D3DCompiler.lib")
// #pragma comment(dll,"d3dcompiler_47.dll")
//const auto d3dCompiler = LoadLibraryEx("d3dcompiler_47.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);