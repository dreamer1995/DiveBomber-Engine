#pragma once
// D3D12 extension library.

#include "DX\d3dx12.h"
#include "..\..\Config\SystemConfig.h"
#include "..\Utility\Common.h"
#include "..\Utility\DEMath.h"
#include "DEWrl.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

// The min/max macros conflict with like-named member functions.
// Only use std::min and std::max defined in <algorithm>.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif