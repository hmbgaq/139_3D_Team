#pragma once

namespace Client
{
	/* 1600 x 900 가능할지도 ㄱㄷ */
	static const unsigned int	g_iWinSizeX = 1280;
	static const unsigned int	g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_TOOL, LEVEL_LOADING, LEVEL_END };

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

		LAYER_END
	};
	
}

using namespace Client;

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

#include "Client_Enum.h"
