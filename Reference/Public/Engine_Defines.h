
 #pragma once

/* Warning Disable */
#pragma warning(disable : 4251) /* dll template export warning */
#pragma warning	(disable : 4819)  /* 한글 주석 경고 */
#pragma warning (disable : 26812) /* enumclass warning */
#pragma warning (disable : 4275) /* XMFLOAT에서 DLL EXPORT 하는 warning  */
//#pragma warning (disable : 26495) /* initialize warning */

/* DX */
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
#include <random>

#include <fstream>
using namespace std;


#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"
#include "Engine_Enum.h"
using namespace Engine;


/* Leak */
#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW

#endif

#endif // _DEBUG



