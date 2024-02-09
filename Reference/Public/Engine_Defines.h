#pragma once

#pragma warning(disable : 4251)

namespace Engine
{
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };
}

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <random>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Effects11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
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

using namespace DirectX;

#include <unordered_map>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
using namespace std;

namespace Engine
{
	static const wchar_t*	g_pTransformTag = TEXT("Com_Transform");
}

#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

#include "Engine_Typedef.h"
using namespace Engine;





#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG




