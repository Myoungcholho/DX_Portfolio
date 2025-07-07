#pragma once

#include <Windows.h>
#include <assert.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <string>
#include <vector>
#include <chrono>
#include <bitset>
#include <cstdlib>

using namespace std;
using namespace chrono;

// MathHelpers
#include "MathHelper.h"

// ComPtr
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// 헤더
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h> // D3DX 대신

// DirectX Tex
#include <DirectXTex.h>
//#pragma comment(lib, "DirectXTex.lib")

// 라이브러리
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Definitions.h"

// ImGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

// Component
#include "Components/Transform.h"

// Core
#include "Core/Math.h"
#include "Core/Color.h"
#include "Core/DynamicMulticastDelegate.h"

// Render
#include "Renders/Material.h"
#include "Renders/MeshData.h"
#include "Renders/Context.h"
#include "Renders/VertexLayout.h"
#include "Renders/CBuffers.h"
#include "Renders/Texture.h"
#include "Renders/Mesh.h"
#include "Renders/CubeMapping.h"

// System
#include "Systems/D3D.h"
#include "Systems/GraphicsDevice.h"
#include "Systems/ImGuiManager.h"
#include "Systems/Timer.h"
#include "Systems/Mouse.h"
#include "Systems/Keyboard.h"

// Viewer
#include "Viewer/Camera.h"

// Utilities
#include "Utilities/String.h"
#include "Utilities/Path.h"

// ModelTools
#include "ModelTools/ModelLoader.h"
#include "ModelTools/GeometryFactory.h"

// Manager
#include "Manager/LightManager.h"
#include "Manager/TextureManager.h"
#include "Manager/Engine.h"

// Billboards
#include "Billboards/BillboardPoint.h"
#include "Billboards/BillboardData.h"
#include "Billboards/IBillboardRenderer.h"
#include "Billboards/BillboardRenderer_GS.h"