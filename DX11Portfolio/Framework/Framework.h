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
#include <memory>

using namespace std;
using namespace chrono;

// MathHelpers
#include "MathHelper.h"
#include "Utilities/D3D11Utils.h"
#include "Components/Transform.h"

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

// Core
#include "Core/Math.h"
#include "Core/Color.h"
#include "Core/DynamicMulticastDelegate.h"

// Render
#include "Renders/GraphicsPSO.h"
#include "Renders/GraphicsCommon.h"
#include "Renders/Material.h"
#include "Renders/CBuffers.h"
#include "Renders/MeshData.h"
#include "Renders/Context.h"
#include "Renders/VertexLayout.h"
#include "Renders/Texture.h"
#include "Renders/Mesh.h"
#include "Renders/CubeMapping.h"
#include "Renders/Model.h"


// PostProcess
#include "PostProcess/ImageFilter.h"
#include "PostProcess/PostProcess.h"

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

// Component
#include "Components/UObject.h"
#include "Components/UActorComponent.h"
#include "Components/AActor.h"
#include "Components/USceneComponent.h"
#include "Components/UPrimitiveComponent.h"
#include "Renders/URenderQueue.h"
#include "Components/UStaticMeshComponent.h"
#include "Components/ULightComponent.h"
#include "Components/UWorld.h"
#include "Components/USkyboxComponent.h"

#include "Renders/URenderer.h"
#include "Core/URenderManager.h"
#include "Core/UGameInstance.h"