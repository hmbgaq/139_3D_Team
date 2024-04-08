#pragma once

#include "Engine_Defines.h"

namespace Client
{
	/* 1600 x 900 가능할지도 ㄱㄷ */
	static const unsigned int	g_iWinSizeY = 720;
	static const unsigned int	g_iWinSizeX = 1280;
	//static const unsigned int	g_iWinSizeX = 1600;
	//static const unsigned int	g_iWinSizeY = 900;
	//static const unsigned int	g_iWinSizeX = 1920;
	//static const unsigned int	g_iWinSizeY = 1080;
	//static const unsigned int	g_iWinSizeX = 2560;
	//static const unsigned int	g_iWinSizeY = 1440;

	// ! Close Window
	_bool g_CloseWindow = false;
	_bool g_UIMouseDownLB = false;
	_bool g_UIMousePressingLB = false;
	_bool g_UIMouseDownRB = false;
	_bool g_UIMousePressingRB = false;

	enum LEVEL { 
		LEVEL_STATIC, 
		LEVEL_LOGO, 

		LEVEL_INTRO,
		LEVEL_INTRO_BOSS,
		LEVEL_SNOWMOUNTAIN,
		LEVEL_SNOWMOUNTAINBOSS,
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
	#define LAYER_BOSS						TEXT("Layer_Boss")
	#define LAYER_PLAYER					TEXT("Layer_Player")
	#define LAYER_UI_MONSTER				TEXT("Layer_UI_Monster")
	#define LAYER_UI_PLAYER					TEXT("Layer_UI_Player")
	#define LAYER_PARTICLE					TEXT("Layer_Particle")

	#define LAYER_PLAYER_BULLET				TEXT("Layer_Player_Bullet")
	#define LAYER_MONSTER_BULLET			TEXT("Layer_Monster_Bullet")


	//Player_Weapon
	#define PLAYER_WEAPON_PUNCH_L			TEXT("Weapon_Punch_L")
	#define PLAYER_WEAPON_PUNCH_R			TEXT("Weapon_Punch_R")
	#define PLAYER_WEAPON_WINCHESTER		TEXT("Weapon_Winchester")
	#define PLAYER_WEAPON_KICK				TEXT("Weapon_Kick")
	#define PLAYER_WEAPON_ZAPPER			TEXT("Weapon_Zapper")
	#define PLAYER_WEAPON_REVOLVER			TEXT("Weapon_Revolver")
	#define PLAYER_WEAPON_SHOTGUN			TEXT("Weapon_Shotgun")
	#define PLAYER_WEAPON_DYNAMITE			TEXT("Weapon_Dynamite")
	#define PLAYER_WEAPON_WHIP				TEXT("Weapon_Whip")



	#define TANK_WEAPON_PUNCH_L				TEXT("Weapon_Punch_L")
	#define TANK_WEAPON_PUNCH_R				TEXT("Weapon_Punch_R")

	#define BANDIT_HEAVY_WEAPON				TEXT("Weapon_Axe")

	#define NPC_WEAPON_EDGAR				TEXT("Weapon_Edgar")

	




	enum class COLLISION_LAYER
	{
		NONE,
		PLAYER_PARRYING,
		MONSTER_SHIELD,
		WEAKNESS,
		EXPLOSION_ATTACK,
		PLAYER_ATTACK,
		MONSTER_ATTACK,
		PLAYER,
		MONSTER,
		DESTRUCTABLE_PROPS,
		OBSTACLE,

		INTERACT,
		PICKING_INSTANCE,
		
		//EDIT_NAVIMESH,
		//DYNAMIC_PROP,
		//BOSS_DYNAMIC_PROP,
		//TRIGGER,
		//LADDER_UP,
		//LADDER_DOWN,
		//DOOR,
		//ELEVATOR,
		//CHECKPOINT,
		//PLAYER_BATCOL,
		//ITEM,
		//ONLY_VIEW,
		//CHECK_DIR,
		//INTERIOR,
		//PICKING_MESH,
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

	enum class GAME_STATE 
	{
		GAMEPLAY,
		UI,
		MAINMENU,
		OPTION,
		SKILLWINDOW,
		CUTSCENE,
		DEAD,
		TALK,
		GAME_STATE_END
	};

	enum class Additional_Skill
	{
		/* Active */
		KICK,			// 발차기
		UPPER_CUT,		// 어퍼컷
		HERO_PUNCH,		// 영웅펀치 (앞으로 강공격)
		HEAL,			// 회복 스킬
		ELECTRIC_DASH,	// 전기대시
		ELECTROCUTE,	// 대시 후 감전 (합선)
		ELECTRIC_WHIP,	// 전기 끌어당기기
		QUAKE_PUNCH,	// 내려찍기 1
		QUAKE_PUNCH2,	// 내려찍기 2
		QUAKE_PUNCH3,	// 내려찍기 3
		HIT_EIGHT,		// E-콤보 (연속 펀치) [감전상태여야 사용가능]
		SUPER_CHARGE,	// 슈퍼차지 모드
		TELEPORT_PUNCH,	// 텔레포트 펀치

		/* Passive */
		MAX_HP,						// 최대 체력 증가 (10%)
		REGEN_HP,					// 체력 재생 (체력이 서서히 회복되는 효과)
		ELECTRIC_DAMAGE_INCREASE,	// 전기스킬 대미지 증가
		ELECTRIC_COOLTIME_DECREASE,	// 전기스킬 쿨타임 감소
		MELEE_DAMAGE_INCREASE,		// 물리스킬 대미지 증가
		INCREASE_EXP,				// 경험치 획득량 증가
		//MELEE_COOLTIME_DECREASE,	// 물리스킬 쿨타임 감소 => INCREASE_EXP !경험치 획득량 증가로 변경
		HASTE,						// 이동속도 증가

		/* Weapon */
		RIFLE,						// 라이플
		REVOLVER,					// 리볼버
		SHOTGUN,					// 샷건

		/* Weapon Passive */
		RIFLE_SKILL1,				// 라이플 스킬1
		REVOLVER_SKILL1,			// 리볼버 스킬1
		SHOTGUN_SKILL1,				// 샷건 스킬1


		Additional_Skill_End
	};

	enum class Additional_Weapon
	{
		REVOLVER,
		RIFLE,
		SHOTGUN,
		Additional_Weapon_End
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
