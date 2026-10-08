#pragma once

#define NOMINMAX

// Standard Library
#include <vector>
#include <format>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>

// Dependencies
#include <d3d11_4.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "core/EngineSettings.h"

#include "imgui/imgui.h"

#include "debug/Profiling.h"

#include "stb/stb_image.h"

// DirectX Classes
#include "core/rendering/d3d11/camera/CameraD3D11.h"
#include "core/rendering/d3d11/camera/CameraOrthographicD3D11.h"

#include "core/rendering/d3d11/buffers/ConstantBufferD3D11.h"
#include "core/rendering/d3d11/buffers/DepthBufferD3D11.h"
#include "core/rendering/d3d11/buffers/GBufferD3D11.h"
#include "core/rendering/d3d11/buffers/IndexBufferD3D11.h"
#include "core/rendering/d3d11/buffers/StructuredBufferD3D11.h"
#include "core/rendering/d3d11/buffers/VertexBufferD3D11.h"

#include "core/rendering/d3d11/mesh/MeshD3D11.h"
#include "core/rendering/d3d11/mesh/SubMeshD3D11.h"

#include "core/rendering/d3d11/resources/InputLayoutD3D11.h"
#include "core/rendering/d3d11/resources/RasterizerD3D11.h"
#include "core/rendering/d3d11/resources/SamplerD3D11.h"
#include "core/rendering/d3d11/resources/ShaderD3D11.h"
#include "core/rendering/d3d11/resources/ShaderResourceTextureD3D11.h"

#include "core/rendering/d3d11/maps/EnviromentMapD3D11.h"
#include "core/rendering/d3d11/maps/ShadowMapD3D11.h"

// Help Classes
#include "core/Time.h"

#include "math/GameMath.h"

#include "platform/D3D11Helper.h"

#include "debug/Debug.h"

#include "core/ThreadPool.h"

#include "assets/ResourceManager.h"
