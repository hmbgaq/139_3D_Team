#pragma once

#include "Engine_Defines.h"

namespace Client
{
	/* 1600 x 900 가능할지도 ㄱㄷ */
	static const unsigned int	g_iWinSizeX = 1280;
	static const unsigned int	g_iWinSizeY = 720;
	//static const unsigned int	g_iWinSizeX = 1600;
	//static const unsigned int	g_iWinSizeY = 900;
	//static const unsigned int	g_iWinSizeX = 1920;
	//static const unsigned int	g_iWinSizeY = 1080;

	enum LEVEL { 
		LEVEL_STATIC, 
		LEVEL_LOGO, 

		LEVEL_INTRO,
		LEVEL_INTRO_BOSS,
		LEVEL_SNOWMOUNTAIN,
		LEVEL_LAVA,
	
		LEVEL_TOOL,
		LEVEL_LOADING,

		LEVEL_GAMEPLAY,

		LEVEL_END };

	enum class OBJECT_LAYER 
	{
		BACKGROUND,
		BUILDING,
		CAMERA,
		EFFECT,
		MONSTER,
		PLAYER,
		UI_MONSTER,
		UI_PLAYER,
		PARTICLE,
		OBJECT_LAYER_END
	};

	#define LAYER_BACKGROUND				TEXT("Layer_BackGround")
	#define LAYER_BUILDING					TEXT("Layer_Building")
	#define LAYER_CAMERA					TEXT("Layer_Camera")
	#define LAYER_EFFECT					TEXT("Layer_Effect")
	#define LAYER_MONSTER					TEXT("Layer_Monster")
	#define LAYER_PLAYER					TEXT("Layer_Player")
	#define LAYER_UI_MONSTER				TEXT("Layer_UI_Monster")
	#define LAYER_UI_PLAYER					TEXT("Layer_UI_Player")
	#define LAYER_PARTICLE					TEXT("Layer_Particle")

	#define LAYER_PLAYER_BULLET				TEXT("Layer_Player_Bullet")


	//Player_Weapon
	#define WEAPON_PUNCH_L					TEXT("Weapon_Punch_L")
	#define WEAPON_PUNCH_R					TEXT("Weapon_Punch_R")
	#define WEAPON_WINCHESTER				TEXT("Weapon_Winchester")



	enum class COLLISION_LAYER
	{
		NONE,
		PLAYER,
		MONSTER,
		PLAYER_ATTACK,
		MONSTER_ATTACK,
		EDIT_NAVIMESH,
		DYNAMIC_PROP,
		BOSS_DYNAMIC_PROP,
		TRIGGER,
		LADDER_UP,
		LADDER_DOWN,
		DOOR,
		ELEVATOR,
		CHECKPOINT,
		PLAYER_BATCOL,
		ITEM,
		ONLY_VIEW,
		CHECK_DIR,
		INTERIOR,
		PICKING_INSTANCE,
		PICKING_MESH,
		LAYER_END
	};

	enum class PHYSX_COLLISION_LAYER
	{
		NONE,
		PLAYER,
		PLAYER_WEAPON,
		MONSTER,
		TERRAIN,
		STATIC_PROP,
		DYNAMIC_PROP,
		DYNAMIC_PEICE,
		GROUND,
		CAMERA,
		INTERIOR,
		LAYER_END
	};


/* Yoojeong_Add */
#define EFFECT_MANAGER CEffect_Manager::GetInstance()
/* Yoojeong_End */
	
}

using namespace Client;

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

#include "Client_Enum.h"
#include "Client_Preset.h"
