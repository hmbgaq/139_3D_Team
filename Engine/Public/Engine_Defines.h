
 #pragma once

/* Warning Disable */
#pragma warning(disable : 4251) /* dll template export warning */
#pragma warning	(disable : 4819)  /* 한글 주석 경고 */
#pragma warning (disable : 26812) /* enumclass warning */
#pragma warning (disable : 4275) /* XMFLOAT에서 DLL EXPORT 하는 warning  */
//#pragma warning (disable : 2532) /* 템플릿 인스턴스화 경고 무시  */
//#pragma warning (disable : 4353) /* 비표준확장 */
#pragma warning (disable : 26495) /* initialize warning */

/* DX */
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <random>

/* Windows SDK */
//#include <mfapi.>
#include <mferror.h>
#include <mfmediacapture.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Effects11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
//#include "DirectXTK\DirectXTex.h"
#include "DirectXTK\ScreenGrab.h"
#include "DirectXTK\SimpleMath.h"

#include "DirectXTK\ScreenGrab.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\Effects.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"

#include "Assimp\scene.h"
#include "assimp\postprocess.h"
#include "Assimp\Importer.hpp"
#include <d3dcompiler.h>
#include "GFSDK_SSAO.h"

using namespace DirectX;

// CUDA 12.0
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

/* Basic*/
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <queue>
#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <mutex>
#include <random>
#include <locale>
#include <codecvt>
#include <fstream>
#include <filesystem>
using namespace std;

#include "Delegate.h"
#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"
#include "Engine_Enum.h"

namespace Engine 
{
	static float					g_iWinsizeX = 1280.f;
	static float					g_iWinsizeY = 720.f;
	//static float					g_iWinsizeX = 1600;
	//static float					g_iWinsizeY = 900.f;
	//static float					g_iWinsizeX = 1920;
	//static float					g_iWinsizeY = 1080.f;

	static float					g_fLightNear = 0.1f;
	static float					g_fLightFar = 3000.f;
	static DirectX::XMFLOAT4		g_vLightPos = { 0.f, 120.f, -105.f, 1.f };
}

using namespace Engine;

// NvCloth
// #include <NvCloth/Factory.h>
// #include <NvCloth/Cloth.h>
// #include <NvCloth/Callbacks.h>
// #include <NvCloth/DxContextManagerCallback.h>
// #include <NvCloth/Fabric.h>
// #include <NvCloth/Solver.h>
// #include <NvCloth/Allocator.h>
// #include <NvCloth/Callbacks.h>
// #include <NvClothExt/ClothFabricCooker.h>
// #include <NvClothExt/ClothMeshDesc.h>
// #include <NvClothExt/ClothMeshQuadifier.h>
// #include <NvClothExt/ClothTetherCooker.h>
// 
// using namespace nv::cloth;
/* for PhysX */
#include "PhysX/PxPhysics.h"
#include "PhysX/PxPhysicsAPI.h"
using namespace physx;

#define IMGUI_DEFIfine IMGUI_DEFINE_MATH_OPERATORfine IMGUI_DEFINE_MATH_OPERATORNE_MATH_OPERATORS


#include "../Fmod/core/fmod.h"
#include "../Fmod/core/fmod.hpp"
#include "../Fmod/core/fmod_errors.h"
#include "../Fmod/studio/fmod_studio.h"
#include "../Fmod/studio/fmod_studio.hpp"
#include "../Fmod/core/fmod_common.h"


/* Leak */
#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "dxgidebug.h"
#define NOMINMAX

#ifndef DBG_NEW
#define PURE_NEW new

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW

#endif



#endif // _DEBUG

#define MAX_CASCADES  3
