#include "stdafx.h"
#include "Loader.h"
#include "GameInstance.h"
#include <process.h>

#pragma region GAMEOBJECT
#include "Camera_Dynamic.h"
#include "SpringCamera.h"
#include "MasterCamera.h"

#include "Weapon_Player.h"
#include "Particle_Blue.h"
#include "Particle_Red.h"
#include "Body_Player.h"
#include "BackGround.h"
#include "Field.h"
#include "ForkLift.h"
#include "Terrain.h"
#include "Monster.h"
#include "Sky.h"
#include "SkyDome.h"
#include "Environment_Object.h"
#include "Environment_Interact.h"
#include "Environment_Instance.h"
#include "Environment_LightObject.h"
#include "Environment_SpecialObject.h"
#include "Screamer.h"
#pragma endregion

#pragma region PLAYER
#include "Player.h"
#include "Body_Player.h"
#include "Weapon_Player.h"
#include "Player_Weapon_Revolver.h"
#include "Player_Weapon_ELShotgun.h"
#include "Player_Weapon_ELWinchester.h"
#include "Player_Weapon_FlameBelcher.h"
#include "Player_Weapon_Shotgun.h"
#include "Player_Weapon_Winchester.h"
#include "Player_Weapon_Punch.h"
#include "Player_Weapon_Kick.h"
#include "Player_Weapon_Zapper.h"

#include "Bullet_Winchester.h"
#include "Bullet_Revolver.h"
#include "Bullet_ELShotgun.h"
#include "Impact_Slam.h"
#pragma endregion

#pragma region VampireCommander
#include "VampireCommander.h"
#include "Body_VampireCommander.h"
#include "VampireCommander_Weapon_Hand.h"
#include "VampireCommander_Projectile_Range1.h"
#include "VampireCommander_Projectile_Range3.h"

#pragma endregion

#pragma region Parasiter
#include "Mother.h"
#include "Body_Mother.h"
#include "Son.h"
#include "Body_Son.h"
#include "Son_Weapon_Head.h"
#include "Son_ColliderBody.h"
#include "Mother_Egg.h"
#include "Son_Projectile.h"
#pragma endregion


#pragma region INFECTED
#include "Infected_A.h"
#include "Infected_B.h"
#include "Infected_C.h"
#include "Body_Infected_A.h"
#include "Body_Infected_B.h"
#include "Body_Infected_C.h"
#include "Weapon_Infected_A.h"
#include "Weapon_Infected_B.h"
#include "Weapon_Infected_C.h"
#pragma endregion

#pragma region ASSASSIN
#include "Assassin.h"
#include "Body_Assassin.h"
#pragma endregion

#pragma region BANDIT_HEAVY
//#include "Bandit_Heavy.h"
//#include "Body_Bandit_Heavy.h"
#pragma endregion

#pragma region Heavy_Vampiric_Zombie
#include "Heavy_Vampiric_Zombie.h"
#include "Body_Heavy_Vampiric_Zombie.h"
#pragma endregion

#pragma region BANDIT_SNIPER
#include "Bandit_Sniper.h"
#include "Body_Bandit_Sniper.h"
#include "Weapon_Bandit_Sniper.h"
#include "Bullet_Bandit_Sniper.h"
#pragma endregion

#pragma region TANK
#include "Tank.h"
#include "Body_Tank.h"
#include "Weapon_Tank.h"
#include "Weapon_Punch_Tank.h"
#include "Projectile_GroundWave.h"

#pragma endregion


#pragma region UI
/* Anything */
#include "UI_Anything.h"
/* Player */
#include "UI_Player_Skill.h"
#include "UI_Player_Skill_Icon.h"
#include "UI_Player_Skill_Frame.h"
#include "UI_Player_Skill_Guige.h"
#include "UI_Player_HP.h"
#include "UI_Player_HPFrame.h"
#include "UI_Player_HPBar.h"
#include "UI_Player_ExpBar.h"
#include "UI_Player_Left_Interface.h"
#include "UI_Player_Right_Interface.h"
#include "UI_Blood.h"
#include "UI_Blood_Vignette.h"
#include "UI_Blood_Veins.h"
#include "UI_Blood_Lights.h"
/* Monster */
#include "UI_MonsterHpFrame.h"
#include "UI_MonsterHp.h"
#include "UI_Weakness.h"
#include "UI_EnemyHP_FrameBar.h"
#include "UI_EnemyHP_Bar.h"
#include "UI_EnemyHP_Shard.h"
#include "UI_EnemyState_Shard.h"
#include "UI_EnemyFrame_Shard.h"
/* TextBox */
#include "UI_TextBox.h"
#include "UI_TutorialBox.h"
#include "UI_QuestBox.h"
#include "UI_QuestIcon.h"
#include "UI_ItemShowcase.h"
#include "UI_ItemIcon.h"
/* Cursor */
#include "UI_MouseCursor.h"
/* Option */
#include "UI_Option_Window.h"
#include "UI_Option_Title_Button.h"
#include "UI_Obtain_Button.h"
#include "UI_ElementList.h"
#include "UI_SelectList.h"
/* LevelUp */
#include "UI_LevelUp_MagicFrame.h"
#include "UI_LevelUp_MagicTrack.h"
#include "UI_LevelUp_Shards.h"
#include "UI_LevelUp_MagicSide.h"
#include "UI_LevelUp_SunBeacon.h"
#include "UI_LevelUp_TextBox.h"
#include "UI_LevelUp_ShieldFrame.h"
/* Aim */
#include "UI_AimCrosshair.h"
/* Distortion */
#include "UI_Distortion.h"
/* BackGround */
#include "UI_BackGround.h"
#include "UI_Loading_Icon.h"
/* Video */
#include "UI_Video.h"
#include "UI_Sprite.h"
/* Interaction */
#include "UI_Interaction.h"
/* MainMenu */
#include "UI_MainMenuList.h"
#pragma endregion UI_END

#pragma region Test
#include "Interact_Chain.h"
#include "InstanceMonster.h"
#pragma endregion

#pragma region Effect
#include "Effect.h"
#include "Effect_Particle.h"
#include "Effect_Rect.h"
#include "Effect_Instance.h"
#include "Effect_Trail.h"
//#include "Effect_Mesh.h"
//#include "Effect_Trail.h"
//#include "Effect_Decal.h"

#include "Grid.h"
#include "Model_Preview.h"
#include "Part_Preview.h"

/* Effect_Legacy */
//#include "Effect_Explosion.h"
//#include "Particle_Blue.h"
//#include "Particle_Red.h"
#pragma endregion

#pragma region PhysX
#include "PhysXCollider.h"
#include "PhysXController.h"
#include "PhysXCharacterController.h"
#pragma endregion

#pragma region Shader
#include "ShaderParsed_Object.h"
#pragma endregion

#include "Imgui_Manager.h" //! 승용 툴에 전달하기위한 모델 태그 셋팅위함

CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

// typedef unsigned(__stdcall* _beginthreadex_proc_type)(void*);

_uint APIENTRY LoadingThread(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}


HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	/* 어떤 레벨의 자원을 로드해야하는지? */
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	/* 스레드를 생성한다. */
	/* LoadingThread : 생성한 스레드가 가장 먼저 호출해야할 함수 */
	/* this : 위 함수를 호출하면서 인자로 전달할 데이터. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

void CLoader::Print_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT hr = 0;

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo_Level();
		break;

	case LEVEL_INTRO:
		hr = Loading_For_Intro_Level();
		break;

	case LEVEL_INTRO_BOSS:
		hr = Loading_For_IntroBoss_Level();
		break;

	case LEVEL_SNOWMOUNTAIN:
		hr = Loading_For_SnowMountain_Level();
		break;

	case LEVEL_SNOWMOUNTAINBOSS:
		hr = Loading_For_SnowMountainBoss_Level();
		break;

	case LEVEL_LAVA:
		hr = Loading_For_Lava_Level();
		break;

	case LEVEL_TOOL:
		hr = Loading_For_Tool_Level();
		break;

	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlay_Level();
		break;
			
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo_Level()
{
	/* 로고 레벨에 필요한 자원을 로드하자. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/10_dickinson_post.png"))));

	///* !UI : MainMenu */
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("MainMenu_Start"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Video/MainMenu/Start/MainMenu_Start (%d).dds"), 729)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("MainMenu_Loop"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Video/MainMenu/Loop/MainMenu_Loop (%d).dds"), 341)));
	
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("MainMenu_GamePlay"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Video/MainMenu/Loop/MainMenu_GamePlay.dds"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("MainMenu_Intro"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Video/MainMenu/Loop/MainMenu_Intro.dds"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("MainMenu_IntroBoss"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Video/MainMenu/Loop/MainMenu_IntroBoss.dds"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("MainMenu_SnowMountain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Video/MainMenu/Loop/MainMenu_SnowMountain.dds"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("MainMenu_SnowMountainBoss"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Video/MainMenu/Loop/MainMenu_SnowMountainBoss.dds"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("MainMenu_Tool"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Video/MainMenu/Loop/MainMenu_Tool.dds"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("MainMenu_Exit"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Video/MainMenu/Loop/MainMenu_Exit.dds"))));

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));
	

	lstrcpy(m_szLoadingText, TEXT("원형객체를(을) 로드하는 중입니다."));
	Ready_Origin();

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level_Origin(LEVEL eLEVEL)
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));

	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Terrain_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/MyMask.dds"), 1)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Terrain_Brush"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Snow"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Explosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Dissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/dissolve_tex.dds"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_DownTop_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/DownTopDask.dds"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_BreakMask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/BreakMask.dds"))));




	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로드하는 중입니다."));

	//! 캐릭터 모델
	_matrix      PivotMatrix;

	/* ------------------ Player ------------------ */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Rentier"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_Shotgun"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/Shotgun/Shotgun", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_ELShotgun"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/ELShotgun/ELShotgun", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_ELWinchester"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/ELWinchester/ELWinchester", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_FlameBelcher"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/FlameBelcher/FlameBelcher", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_Winchester"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/Winchester/Winchester", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_Revolver"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/Revolver/PlayerRevolver", PivotMatrix)));
	FAILED_CHECK(Loading_Player(eLEVEL));


	/* ------------------ Monster ------------------ */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Screamer"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Screamer/Screamer", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/A/Infected_A", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/B/Infected_B", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/C/Infected_C", PivotMatrix)));
	//AILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/D/Infected_D", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Bandit_Sniper"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Sniper/Model/Bandit_Sniper", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Bandit_Sniper_Weapon"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Bandit_Sniper/Weapon/Sniper_Weapon", PivotMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_BanditA"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit/A/Bandit_A", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_BanditB"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit/B/Bandit_B", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_BanditC"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit/C/Bandit_C", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_BanditD"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit/D/Bandit_D", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Assassin"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Assassin/ReverseFootBase_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_E"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/E/Infected_E", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_Projectile"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Infected/Projectile/LightBug", PivotMatrix)));
	
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Bandit_Heavy"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Heavy/ManHeavyBase_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Bandit_Heavy_Vampiric"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Heavy/Bandit_Heavy_Vampiric/Model/Bandit_Heavy_Vampiric", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Bandit_Heavy_Vampiric_Weapon"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Bandit_Heavy/Bandit_Heavy_Vampiric/Weapon/TwoHandedAxe", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_BooHag"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/BooHag/ReverseFootBase_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Digger"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Digger/ReverseFootBase_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Gambit"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Gambit/Gambit_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Gambit_Egg"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Gambit/GambitEgg/Gambit_WeakspotEgg_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Gambit_XRay"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Gambit/XRay/Gambit_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Ranger"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Ranger/Normal/Ranger_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Ranger_Finisher"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Ranger/Finisher/Ranger_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Stalker"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Stalker/Normal/ReverseFootBase_Skeleton", PivotMatrix)));

	//HERE
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Heavy_Vampiric_Zombie"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Heavy_Vampiric_Zombie/Heavy_Vampiric_Zombie", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Tank"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Tank/Tank", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Heavy_Vampiric_Zombie_Weapon"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Heavy_Vampiric_Zombie/Weapon/Axe", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Tank_Weapon"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Tank/Shield/Shield", PivotMatrix)));

	
	/* ------------------ Boss ------------------ */
	//TODO VampireCommander
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_VampireCommander"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/VampireCommander/VampireCommander", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_BeastBoss_Phase1"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Beast/Phase1/Beast_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_BeastBoss_Phase2"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Beast/Phase2/Beast_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_BeastBoss_Phase3"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Beast/Phase3/Beast_Skeleton", PivotMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Effect_BatStorm_01"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/VampireCommander/BatStorm/BatStorm_01/BatStorm_01", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Effect_BatStorm_02"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/VampireCommander/BatStorm/BatStorm_02/BatStorm_02", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Effect_BeastSkull"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/VampireCommander/BeastSkull/BeastSkull", PivotMatrix)));
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);	// Impact 모델은 180도 돌리면 안됨
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Effect_Impact"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/VampireCommander/Impact/Impact", PivotMatrix)));
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* ------------------ 상호작용 ------------------ */
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Chain"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Chain/Chain", PivotMatrix)));

	/* ------------------------------------------ */
	//TODO보류 애니메이션 찾아야됨
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Spider"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Stalker/Spider/StalkerSpider01_Death_01", PivotMatrix)));
	//PivotMatrix = XMMatrixScaling(1.3f, 1.3f, 1.3f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_SkyDome"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/SkyDome/EngineSky", PivotMatrix)));
	
	//!환경 모델
	
	

	//!버퍼 
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Particle_Rect"), CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, 100)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Particle_Point"), CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, 500)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Environment_Model_Instance"), CVIBuffer_Environment_Model_Instance::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_AnimModel_Instance"), CVIBuffer_AnimModel_Instance::Create(m_pDevice, m_pContext)));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));
	Loading_For_Shader(eLEVEL);

	lstrcpy(m_szLoadingText, TEXT("네비게이션를(을) 로드하는 중입니다."));
	if (eLEVEL == LEVEL_GAMEPLAY)
	{
		FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Data_Map/Navigation/Stage1NavigationFinal.dat"))));
	}
	else if (eLEVEL == LEVEL_SNOWMOUNTAIN) //! 임시
	{
		FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Data_Map/Navigation/Stage1NavigationFinal.dat"))));
	}

	lstrcpy(m_szLoadingText, TEXT("콜라이더를(을) 로드하는 중입니다."));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_RigidBody"), CRigidBody::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB)));	
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE)));

	lstrcpy(m_szLoadingText, TEXT("피직스를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_PhysXController"), CPhysXController::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_PhysXCharacterController"), CPhysXCharacterController::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_PhysXCollider"), CPhysXCollider::Create(m_pDevice, m_pContext)));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
		
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level()
{	
	lstrcpy(m_szLoadingText, TEXT("맵을 로드하는 중입니다."));

	Ready_Environment_Model(LEVEL_GAMEPLAY);


	lstrcpy(m_szLoadingText, TEXT("이펙트를 로드하는 중입니다."));

	FAILED_CHECK(Ready_Effect_All(LEVEL_GAMEPLAY));		//! 이펙트 로드


	FAILED_CHECK(Loading_For_GamePlay_Level_Origin(LEVEL_GAMEPLAY));

	return S_OK;
}

HRESULT CLoader::Loading_For_Intro_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Texture_RaidPoolGround_Diffuse1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_BloodGround_ORM.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Texture_RaidPoolLight1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_RaidBloodlight_01_BC.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Texture_RaidPoolLight2"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_RaidBloodlight_02_BC.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Texture_RaidPoolLight3"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_RaidBloodlight_03_BC.dds"))));

	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Texture_Effect_Mask_Waves"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/02_Mask/Waves/Mask_Wave_%d.dds"), 6)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Texture_Effect_Noise"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/03_Noise/Noise_%d.dds"), 25)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Texture_Effect_Sprite"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/04_Sprites/Sprite_%d.dds"), 24)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Texture_Effect_Sprite_Smokes"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/04_Sprites/Smokes/Sprite_smoke_%d.dds"), 34)));

	lstrcpy(m_szLoadingText, TEXT("맵을 로드하는 중입니다."));
	Ready_Environment_Model(LEVEL_INTRO);

	lstrcpy(m_szLoadingText, TEXT("네비게이션를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Data_Map/Navigation/IntroBossNaviFinal.dat"))));

	_matrix      PivotMatrix;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	lstrcpy(m_szLoadingText, TEXT("플레이어를 로드하는 중입니다."));
	FAILED_CHECK(Loading_Player(LEVEL_INTRO));

	lstrcpy(m_szLoadingText, TEXT("몬스터를 로드하는 중입니다."));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Model_VampireCommander"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/VampireCommander/VampireCommander", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Model_VampireCommander_Projectile_Range1"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/BeastSkull/BeastSkull", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Model_VampireCommander_Projectile_Range3"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/BeastSkull/BeastSkull", PivotMatrix)));

	/* ---------------------- Test Model 추가 라인 ---------------------- */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Model_Infected_A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/A/Infected_A", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Model_Infected_B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/B/Infected_B", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Model_Infected_C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/C/Infected_C", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Model_Bandit_Sniper"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Sniper/Model/Bandit_Sniper", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Model_Bandit_Sniper_Weapon"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Bandit_Sniper/Weapon/Sniper_Weapon", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Model_LootCreate_Small"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LootCreate/Chest_OpenedLootCrate_Small", PivotMatrix)));

	/* ------------------------------------------------------------------ */

	lstrcpy(m_szLoadingText, TEXT("이펙트를 로드하는 중입니다."));
	FAILED_CHECK(Ready_Effect_All(LEVEL_INTRO));		//! 이펙트 로드


	lstrcpy(m_szLoadingText, TEXT("아이템, 장비를 로드하는 중입니다."));
	
	lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext))); //! 스카이박스
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_VIBuffer_Environment_Model_Instance"), CVIBuffer_Environment_Model_Instance::Create(m_pDevice, m_pContext)));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));
	Loading_For_Shader(LEVEL_INTRO);

	lstrcpy(m_szLoadingText, TEXT("콜라이더를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE)));

	lstrcpy(m_szLoadingText, TEXT("피직스를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_PhysXController"), CPhysXController::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_PhysXCharacterController"), CPhysXCharacterController::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO, TEXT("Prototype_Component_PhysXCollider"), CPhysXCollider::Create(m_pDevice, m_pContext)));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_IntroBoss_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5)));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Texture_RaidPoolGround_Diffuse1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_BloodGround_ORM.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Texture_RaidPoolLight1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_RaidBloodlight_01_BC.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Texture_RaidPoolLight2"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_RaidBloodlight_02_BC.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Texture_RaidPoolLight3"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_RaidBloodlight_03_BC.dds"))));


	lstrcpy(m_szLoadingText, TEXT("맵을 로드하는 중입니다."));
	Ready_Environment_Model(LEVEL_INTRO_BOSS);

	lstrcpy(m_szLoadingText, TEXT("네비게이션를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Data_Map/Navigation/IntroBossNaviFinal.dat"))));

	_matrix      PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	lstrcpy(m_szLoadingText, TEXT("플레이어를 로드하는 중입니다."));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Model_Rentier"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player", PivotMatrix)));
	FAILED_CHECK(Loading_Player(LEVEL_INTRO_BOSS));

	lstrcpy(m_szLoadingText, TEXT("몬스터를 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Model_VampireCommander"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/VampireCommander/VampireCommander", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Model_Infected_A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/A/Infected_A", PivotMatrix)));


#pragma region 이펙트 로드_INTRO_BOSS

	lstrcpy(m_szLoadingText, TEXT("이펙트를 로드하는 중입니다."));
	FAILED_CHECK(Ready_Effect_All(LEVEL_INTRO_BOSS));		//! 이펙트 로드

#pragma endregion 이펙트 로드_INTRO_BOSS 끝


	lstrcpy(m_szLoadingText, TEXT("아이템, 장비를 로드하는 중입니다."));
	//! Player Weapon
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Model_Player_Weapon_Winchester"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/Winchester/Winchester", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Model_Player_Weapon_Revolver"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/Revolver/PlayerRevolver", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Model_Player_Weapon_ELWinchester"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/ELWinchester/ELWinchester", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Model_Player_Weapon_ELShotgun"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/ELShotgun/ELShotgun", PivotMatrix)));


	//! BOSS Projectile
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Model_VampireCommander_Projectile_Range1"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/BeastSkull/BeastSkull", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Model_VampireCommander_Projectile_Range3"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/BeastSkull/BeastSkull", PivotMatrix)));


	lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext))); //! 스카이박스
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_VIBuffer_Environment_Model_Instance"), CVIBuffer_Environment_Model_Instance::Create(m_pDevice, m_pContext)));

	
	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));
	Loading_For_Shader(LEVEL_INTRO_BOSS);
	
	lstrcpy(m_szLoadingText, TEXT("콜라이더를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE)));

	lstrcpy(m_szLoadingText, TEXT("피직스를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_PhysXController"), CPhysXController::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_PhysXCharacterController"), CPhysXCharacterController::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_PhysXCollider"), CPhysXCollider::Create(m_pDevice, m_pContext)));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_SnowMountain_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5)));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Texture_RaidPoolGround_Diffuse1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_BloodGround_ORM.dds"))));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Texture_SpecialSignalDiffuseTexture"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/map/SnowMountain/NonAnim/Single/TeslaRICell/T_RailroadSwitch_01_BC.png"))));

	lstrcpy(m_szLoadingText, TEXT("맵을 로드하는 중입니다."));
	Ready_Environment_Model(LEVEL_SNOWMOUNTAIN);

	lstrcpy(m_szLoadingText, TEXT("네비게이션를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Data_Map/Navigation/IntroBossNaviFinal.dat"))));

	_matrix      PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	lstrcpy(m_szLoadingText, TEXT("플레이어를 로드하는 중입니다."));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Model_Rentier"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player", PivotMatrix)));
	FAILED_CHECK(Loading_Player(LEVEL_SNOWMOUNTAIN));

	lstrcpy(m_szLoadingText, TEXT("몬스터를 로드하는 중입니다."));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Model_Mother"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Parasiter/Mother/Mother", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Model_Son"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Parasiter/Son/Son", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Model_Infected_A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/A/Infected_A", PivotMatrix)));



#pragma region 이펙트 로드_SNOWMOUNTAIN

	lstrcpy(m_szLoadingText, TEXT("이펙트를 로드하는 중입니다."));
	FAILED_CHECK(Ready_Effect_All(LEVEL_SNOWMOUNTAIN));		//! 이펙트 로드

#pragma endregion 이펙트 로드_SNOWMOUNTAIN 끝


	lstrcpy(m_szLoadingText, TEXT("아이템, 장비를 로드하는 중입니다."));
	//! Player Weapon
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Model_Player_Weapon_Shotgun"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/Shotgun/Shotgun", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Model_Player_Weapon_ELShotgun"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/ELShotgun/ELShotgun", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Model_Player_Weapon_ELWinchester"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/ELWinchester/ELWinchester", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Model_Player_Weapon_FlameBelcher"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/FlameBelcher/FlameBelcher", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Model_Player_Weapon_Winchester"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/Winchester/Winchester", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Model_Player_Weapon_Revolver"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/Revolver/PlayerRevolver", PivotMatrix)));




	//! BOSS Projectile
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Model_VampireCommander_Projectile_Range1"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/BeastSkull/BeastSkull", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Model_VampireCommander_Projectile_Range3"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/BeastSkull/BeastSkull", PivotMatrix)));


	lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext))); //! 스카이박스
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_VIBuffer_Environment_Model_Instance"), CVIBuffer_Environment_Model_Instance::Create(m_pDevice, m_pContext)));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));
	Loading_For_Shader(LEVEL_SNOWMOUNTAIN);

	lstrcpy(m_szLoadingText, TEXT("콜라이더를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE)));

	lstrcpy(m_szLoadingText, TEXT("피직스를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_PhysXController"), CPhysXController::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_PhysXCharacterController"), CPhysXCharacterController::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAIN, TEXT("Prototype_Component_PhysXCollider"), CPhysXCollider::Create(m_pDevice, m_pContext)));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_SnowMountainBoss_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5)));
	
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Texture_RaidPoolGround_Diffuse1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_BloodGround_ORM.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Texture_RaidPoolLight1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_RaidBloodlight_01_BC.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Texture_RaidPoolLight2"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_RaidBloodlight_02_BC.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Texture_RaidPoolLight3"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_RaidBloodlight_03_BC.dds"))));


	lstrcpy(m_szLoadingText, TEXT("맵을 로드하는 중입니다."));
	Ready_Environment_Model(LEVEL_SNOWMOUNTAINBOSS);

	lstrcpy(m_szLoadingText, TEXT("네비게이션를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Data_Map/Navigation/SnowBossTestNavi.dat"))));

	_matrix      PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	lstrcpy(m_szLoadingText, TEXT("플레이어를 로드하는 중입니다."));
	FAILED_CHECK(Loading_Player(LEVEL_SNOWMOUNTAINBOSS));

	lstrcpy(m_szLoadingText, TEXT("몬스터를 로드하는 중입니다."));
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Model_Mother"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Parasiter/Mother/Mother", PivotMatrix)));
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Model_Son"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Parasiter/Son/Son", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Model_Infected_A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/A/Infected_A", PivotMatrix)));


#pragma region 이펙트 로드_SNOWMOUNTAINBOSS

	lstrcpy(m_szLoadingText, TEXT("이펙트를 로드하는 중입니다."));
	FAILED_CHECK(Ready_Effect_All(LEVEL_SNOWMOUNTAINBOSS));		//! 이펙트 로드

#pragma endregion 이펙트 로드_SNOWMOUNTAINBOSS 끝

	lstrcpy(m_szLoadingText, TEXT("아이템, 장비를 로드하는 중입니다."));

	//! BOSS Projectile
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Model_Mother_Egg"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Boss/Parasiter/Egg/Egg", PivotMatrix)));
	PivotMatrix = XMMatrixScaling(0.7f, 0.7f, 0.7f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Model_Son_Projectile"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Boss/Parasiter/SonEgg/SonEgg", PivotMatrix)));


	lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext))); //! 스카이박스
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_VIBuffer_Environment_Model_Instance"), CVIBuffer_Environment_Model_Instance::Create(m_pDevice, m_pContext)));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));
	Loading_For_Shader(LEVEL_SNOWMOUNTAINBOSS);

	lstrcpy(m_szLoadingText, TEXT("콜라이더를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE)));

	lstrcpy(m_szLoadingText, TEXT("피직스를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_PhysXController"), CPhysXController::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_PhysXCharacterController"), CPhysXCharacterController::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_SNOWMOUNTAINBOSS, TEXT("Prototype_Component_PhysXCollider"), CPhysXCollider::Create(m_pDevice, m_pContext)));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Lava_Level()
{
	FAILED_CHECK(Loading_For_GamePlay_Level_Origin(LEVEL_LAVA));

	return S_OK;
}

HRESULT CLoader::Loading_For_Tool_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5)));


	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_SpecialSignalDiffuseTexture"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/map/SnowMountain/NonAnim/Single/TeslaRICell/T_RailroadSwitch_01_BC.png"))));

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));

	//! Player
	_matrix		PivotMatrix = {};
	//FAILED_CHECK(Loading_Player(LEVEL_TOOL));

#pragma region 캐릭터 모델 : 주석 풀고 병합해야함!!!
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(Loading_Player(LEVEL_TOOL));

	//TODO VampireCommander
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_VampireCommander"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/VampireCommander/VampireCommander", PivotMatrix)));
	//TODO Parasiter
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Mother"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Parasiter/Mother/Mother", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Son"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Parasiter/Son/Son", PivotMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Infected_A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/A/Infected_A", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Infected_B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/B/Infected_B", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Infected_C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/C/Infected_C", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Bandit_Sniper"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Sniper/Model/Bandit_Sniper", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Bandit_Sniper_Weapon"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Bandit_Sniper/Weapon/Sniper_Weapon", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Infected_B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/B/Infected_B", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Infected_C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/C/Infected_C", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Bandit_Sniper"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Sniper/Model/Bandit_Sniper", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Bandit_Sniper_Weapon"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Bandit_Sniper/Weapon/Sniper_Weapon", PivotMatrix)));


	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Heavy_Vampiric_Zombie"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Heavy_Vampiric_Zombie/Heavy_Vampiric_Zombie", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Tank"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Tank/Tank", PivotMatrix)));
	
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Heavy_Vampiric_Zombie_Weapon"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Heavy_Vampiric_Zombie/Weapon/Axe", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Tank_Weapon"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Tank/Shield/Shield", PivotMatrix)));


	//!//! 터짐 텍스처 찾아야됨 FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_E"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/E/CharacterBase_Skeleton", PivotMatrix)));
	//!
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Assassin"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Assassin/ReverseFootBase_Skeleton", PivotMatrix)));
	//!
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_BeastBoss_Phase1"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Beast/Phase1/Beast_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_BeastBoss_Phase2"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Beast/Phase2/Beast_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_BeastBoss_Phase3"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Beast/Phase3/Beast_Skeleton", PivotMatrix)));
	//!
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Bandit_Heavy"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Heavy/ManHeavyBase_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Bandit_Heavy_Vampiric"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Heavy/Bandit_Heavy_Vampiric/ManHeavyBase_Skeleton", PivotMatrix)));
	//!//
	//! 캐릭터 모델

	/* ------------------ Player ------------------ */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_BooHag"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/BooHag/ReverseFootBase_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Digger"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Digger/ReverseFootBase_Skeleton", PivotMatrix)));
	//!//
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Gambit"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Gambit/Gambit_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Gambit_Egg"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Gambit/GambitEgg/Gambit_WeakspotEgg_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Gambit_XRay"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Gambit/XRay/Gambit_Skeleton", PivotMatrix)));
	//!
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Ranger"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Ranger/Normal/Ranger_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Ranger_Finisher"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Ranger/Finisher/Ranger_Skeleton", PivotMatrix)));
	//!//
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Stalker"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Stalker/Normal/ReverseFootBase_Skeleton", PivotMatrix)));

	//TODO보류 애니메이션 찾아야됨
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Spider"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Stalker/Spider/StalkerSpider01_Death_01", PivotMatrix)));
#pragma endregion 캐릭터 모델 : 주석 풀고 병합해야함!!! 끝 

#pragma region 환경 : 주석 풀고 병합해야함!!!
	//! 환경 모델
	Ready_Environment_Model(LEVEL_TOOL);
#pragma endregion 환경 : 주석 풀고 병합해야함!!! 끝 

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Chain"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Chain/Chain", PivotMatrix)));


#pragma region 이펙트 로드
	lstrcpy(m_szLoadingText, TEXT("이펙트를 로드하는 중입니다."));
	FAILED_CHECK(Ready_Effect_All(LEVEL_TOOL));

	//! 테스트용 이펙트 모델 
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Xray_ManHeavy"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Xray/Xray_ManHeavy", PivotMatrix)));

	//! 이펙트 툴용 모델
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_EffectTool_IntroBoss_Floor"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/map/Stage1BossMap/NonAnim/Single/AncientTempleJapaneseShrineFloor1", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Effect_Torch"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Torch/Torch", PivotMatrix)));		// 불 붙일 횃불 모델 테스트

	//! 이펙트 툴용 버퍼
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Grid"), CVIBuffer_Grid::Create(m_pDevice, m_pContext)));	 
#pragma endregion 이펙트 로드 끝


	//!버퍼 
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Field"), CVIBuffer_Field::Create(m_pDevice, m_pContext))); //! 툴용 버퍼
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Point"), CVIBuffer_Point::Create(m_pDevice, m_pContext, 500)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Environment_Model_Instance"), CVIBuffer_Environment_Model_Instance::Create(m_pDevice, m_pContext)));



	//!셰이더
	Loading_For_Shader(LEVEL_TOOL);

	//!콜라이더
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_RigidBody"), CRigidBody::Create(m_pDevice, m_pContext)));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE)));


	lstrcpy(m_szLoadingText, TEXT("피직스를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_PhysXController"), CPhysXController::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_PhysXCharacterController"), CPhysXCharacterController::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_PhysXCollider"), CPhysXCollider::Create(m_pDevice, m_pContext)));
	

	lstrcpy(m_szLoadingText, TEXT("네비게이션를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Data_Map/Navigation/NaviEmpty.dat"))));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_PhysXCharacterController"), CPhysXCharacterController::Create(m_pDevice, m_pContext)));

	Ready_UI_Tool_Texture(LEVEL_TOOL);

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_Player(LEVEL eLEVEL)
{
	_matrix      PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Rentier"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_Shotgun"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/Shotgun/Shotgun", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_ELShotgun"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/ELShotgun/ELShotgun", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_ELWinchester"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/ELWinchester/ELWinchester", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_FlameBelcher"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/FlameBelcher/FlameBelcher", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_Winchester"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/Winchester/Winchester", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_Revolver"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/Revolver/PlayerRevolver", PivotMatrix)));

	return S_OK;
}

HRESULT CLoader::Loading_For_Shader(LEVEL eLevel)
{
	/* Texture */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Shader_Dissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/Dissolve/dissolve_tex.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Shader_IceNoise"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/Icicle/T_Ice_Noise.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Shader_IceDiffuse"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/Icicle/Temp.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Shader_IcarusRAD"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/Icarus/RAD.dds"))));
	
	/* Shader */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Model"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Monster"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Monster.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_VtxCube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_AnimModel"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Particle_Rect"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Rect.hlsl"), VTX_PARTICLE_RECT::Elements, VTX_PARTICLE_RECT::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Particle_Point"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Point.hlsl"), VTX_PARTICLE_POINT::Elements, VTX_PARTICLE_POINT::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Model_Instance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModelInstance.hlsl"), VTXMODEL_INSTANCE_DECLARATION::Elements, VTXMODEL_INSTANCE_DECLARATION::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_AnimModel_Instance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModelInstance.hlsl"), VTXMODEL_ANIMMODEL_INSTANCE_DECLARATION::Elements, VTXMODEL_ANIMMODEL_INSTANCE_DECLARATION::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Effect_Model_Instance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectModelInstance.hlsl"), VTXMODEL_INSTANCE_DECLARATION::Elements, VTXMODEL_INSTANCE_DECLARATION::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_EffectTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_EffectTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)));

	return S_OK;
}

HRESULT CLoader::Ready_Origin()
{
	/* Logo 원형객체 */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_BackGround"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Field"), CField::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Field"))));
	//! Player
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"), CBody_Player::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Player"))));
	
	//! PlayerWeapon
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapon_Revolver"), CPlayer_Weapon_Revolver::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Weapon_Revolver"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapon_ELShotgun"), CPlayer_Weapon_ELShotgun::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Weapon_ELShotgun"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapon_ELWinchester"), CPlayer_Weapon_ELWinchester::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Weapon_ELWinchester"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapon_FlameBelcher"), CPlayer_Weapon_FlameBelcher::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Weapon_FlameBelcher"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapon_Shotgun"), CPlayer_Weapon_Shotgun::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Weapon_Shotgun"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapon_Winchester"), CPlayer_Weapon_Winchester::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Weapon_Winchester"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapon_Punch"), CPlayer_Weapon_Punch::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Weapon_Punch"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapon_Kick"), CPlayer_Weapon_Kick::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Weapon_Kick"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapon_Zapper"), CPlayer_Weapon_Zapper::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Weapon_Zapper"))));

	//! PlayerBullet
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_Winchester"), CBullet_Winchester::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Bullet_Winchester"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_Revolver"), CBullet_Revolver::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Bullet_Revolver"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_ELShotgun"), CBullet_ELShotgun::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Bullet_ELShotgun"))));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Impact_Slam"), CImpact_Slam::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Impact_Slam"))));


	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Player"), CWeapon_Player::Create(m_pDevice, m_pContext)));
	//! =====================================Boss Line=========================================
	//TODO VampireCommander
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VampireCommander"), CVampireCommander::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_VampireCommander"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_VampireCommander"), CBody_VampireCommander::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_VampireCommander"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VampireCommander_Weapon_Hand"), CVampireCommander_Weapon_Hand::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_VampireCommander_Weapon_Hand"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VampireCommander_Projectile_Range1"), CVampireCommander_Projectile_Range1::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_VampireCommander_Projectile_Range1"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VampireCommander_Projectile_Range3"), CVampireCommander_Projectile_Range3::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_VampireCommander_Projectile_Range3"))));
	//TODO PARASITER
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mother"), CMother::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Mother"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Son"), CSon::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Son"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mother_Egg"), CMother_Egg::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Mother_Egg"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Son_Projectile"), CSon_Projectile::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Son_Projectile"))));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Mother"), CBody_Mother::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Mother"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Son"), CBody_Son::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Son"))));
	
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Son_Weapon_Head"), CSon_Weapon_Head::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Son_Weapon_Head"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Son_ColliderBody"), CSon_ColliderBody::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Son_ColliderBody"))));

	//! ===================================== Monster Line=========================================

	//! Infected
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Infected_A"), CInfected_A::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Infected_A"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Infected_B"), CInfected_B::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Infected_B"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Infected_C"), CInfected_C::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Infected_C"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Infected_A"), CBody_Infected_A::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Infected_A"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Infected_B"), CBody_Infected_B::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Infected_B"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Infected_C"), CBody_Infected_C::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Infected_C"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Infected_A"), CWeapon_Infected_A::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Weapon_Infected_A"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Infected_B"), CWeapon_Infected_B::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Weapon_Infected_B"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Infected_C"), CWeapon_Infected_C::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Weapon_Infected_C"))));

	//! Assassin
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Assassin"), CAssassin::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Assassin"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Assassin"), CBody_Assassin::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Assassin"))));

	//! Bandit_Heavy
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bandit_Heavy"), CBandit_Heavy::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Bandit_Heavy"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Bandit_Heavy"), CBody_Bandit_Heavy::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Bandit_Heavy"))));

	//! Heavy_Vampiric_Zombie
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Heavy_Vampiric_Zombie"), CBody_Heavy_Vampiric_Zombie::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Heavy_Vampiric_Zombie"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Tank"), CWeapon_Tank::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Weapon_Tank"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Punch_Tank"), CWeapon_Punch_Tank::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Weapon_Punch_Tank"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Heavy_Vampiric_Zombie"), CHeavy_Vampiric_Zombie::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Heavy_Vampiric_Zombie"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Projectile_GroundWave"), CProjectile_GroundWave::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Projectile_GroundWave"))));



	//! Bandit_Sniper
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bandit_Sniper"), CBandit_Sniper::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Bandit_Sniper"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Bandit_Sniper"), CBody_Bandit_Sniper::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Bandit_Sniper"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Bandit_Sniper"), CWeapon_Bandit_Sniper::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Weapon_Bandit_Sniper"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_Bandit_Sniper"), CBullet_Bandit_Sniper::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Bullet_Bandit_Sniper"))));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Tank"), CBody_Tank::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Tank"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Tank"), CWeapon_Tank::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Weapon_Tank"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Punch_Tank"), CWeapon_Punch_Tank::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Weapon_Punch_Tank"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tank"), CTank::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Tank"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Projectile_GroundWave"), CProjectile_GroundWave::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Projectile_GroundWave"))));



	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Weapon_Winchester"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"), CMonster::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Monster"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"), CForkLift::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_ForkLift"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Screamer"), CScreamer::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Screamer"))));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InstanceMonster"), CInstanceMonster::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_InstanceMonster"))));

	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"), CCamera_Dynamic::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Camera_Dynamic"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Spring"), CSpringCamera::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Camera_Spring"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MasterCamera"), CMasterCamera::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_MasterCamera"))));


	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Sky"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyDome"), CSkyDome::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_SkyDome"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Blue"), CParticle_Blue::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Particle_Blue"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Red"), CParticle_Red::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Particle_Red"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Explosion"), CEffect_Explosion::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Explosion"))));


	//! 상호작용 없는 환경
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_Instance"), CEnvironment_Instance::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Environment_Instance"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_Object"), CEnvironment_Object::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Environment_Object"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_InteractObject"), CEnvironment_Interact::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Environment_InteractObject"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_LightObject"), CEnvironment_LightObject::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Environment_LightObject"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_SpecialObject"), CEnvironment_SpecialObject::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Environment_SpecialObject"))));
	
	//!성희
	Ready_UI_Origin();

	//! 상호작용 환경
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Interact_Chain"), CInteract_Chain::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Interact_Chain"))));

#pragma region Effect
	//!유정
	FAILED_CHECK(Ready_Effect_Origin());

	//!유정 : For.Effect Tool
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grid"), CGrid::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Grid"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Preview"), CModel_Preview::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Model_Preview"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Preview"), CPart_Preview::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Part_Preview"))));
#pragma endregion

	//!소영 - ShaderTool 
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShaderParsed_Object"), CShaderParsed_Object::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_ShaderParsed_Object"))));

	return S_OK;
}

HRESULT CLoader::Ready_UI_Origin()
{
#pragma region Anything
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Anything"), CUI_Anything::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Anything"))));
#pragma endregion End

#pragma region Player
	/* HP */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Hp"), CUI_Player_HP::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Player_Hp"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_HPBar"), CUI_Player_HPBar::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Player_HPBar"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_HPFrame"), CUI_Player_HPFrame::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Player_HPFrame"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_ExpBar"), CUI_Player_ExpBar::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Player_ExpBar"))));
	
	/* Player_Interface */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Left_Interface"), CUI_Player_Left_Interface::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Player_Left_Interface"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Right_Interface"), CUI_Player_Right_Interface::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Player_Right_Interface"))));
	
	/* Blood */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Blood"), CUI_Blood::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Blood"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Blood_Lights"), CUI_Blood_Lights::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Blood_Lights"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Blood_Veins"), CUI_Blood_Veins::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Blood_Veins"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Blood_Vignette"), CUI_Blood_Vignette::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Blood_Vignette"))));
	
	/* Skill */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Skill"), CUI_Player_Skill::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Player_Skill"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Skill_Frame"), CUI_Player_Skill_Frame::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Player_Skill_Frame"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Skill_Guige"), CUI_Player_Skill_Guige::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Player_Skill_Guige"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Skill_Icon"), CUI_Player_Skill_Icon::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Player_Skill_Icon"))));
#pragma endregion End

#pragma region Boss
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_EnemyHP_FrameBar"), CUI_EnemyHP_FrameBar::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_EnemyHP_FrameBar"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_EnemyHP_Bar"), CUI_EnemyHP_Bar::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_EnemyHP_Bar"))));
#pragma endregion End

#pragma region Monster
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHpFrame"), CUI_MonsterHpFrame::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_MonsterHpFrame"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHp"), CUI_MonsterHp::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_MonsterHp"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Weakness"), CUI_Weakness::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Weakness"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_EnemyHP_Shard"), CUI_EnemyHP_Shard::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_EnemyHP_Shard"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_EnemyState_Shard"), CUI_EnemyState_Shard::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_EnemyState_Shard"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_EnemyFrame_Shard"), CUI_EnemyFrame_Shard::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_EnemyFrame_Shard"))));
#pragma endregion End

#pragma region TutorialBox
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TutorialBox"), CUI_TutorialBox::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_TutorialBox"))));
#pragma endregion End

#pragma region QuestBox
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QuestBox"), CUI_QuestBox::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_QuestBox"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QuestIcon"), CUI_QuestBox::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_QuestIcon"))));
#pragma endregion End

#pragma region RewardBox
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ItemShowcase"), CUI_ItemShowcase::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_ItemShowcase"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ItemIcon"), CUI_ItemIcon::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_ItemIcon"))));
#pragma endregion End

#pragma region LevelUp
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LevelUp_MagicFrame"), CUI_LevelUp_MagicFrame::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_LevelUp_MagicFrame"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LevelUp_MagicTrack"), CUI_LevelUp_MagicTrack::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_LevelUp_MagicTrack"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LevelUp_Shards"), CUI_LevelUp_Shards::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_LevelUp_Shards"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LevelUp_MagicSide"), CUI_LevelUp_MagicSide::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_LevelUp_MagicSide"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LevelUp_SunBeacon"), CUI_LevelUp_SunBeacon::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_LevelUp_SunBeacon"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LevelUp_TextBox"), CUI_LevelUp_TextBox::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_LevelUp_TextBox"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LevelUp_ShieldFrame"), CUI_LevelUp_ShieldFrame::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_LevelUp_ShieldFrame"))));
#pragma endregion End

#pragma region Option
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Option_Window"), CUI_Option_Window::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Option_Window"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Option_Title_Button"), CUI_Option_Title_Button::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Option_Title_Button"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ElementList"), CUI_ElementList::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_ElementList"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_SelectList"), CUI_SelectList::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_SelectList"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Obtain_Button"), CUI_Obtain_Button::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Obtain_Button"))));
#pragma endregion End

#pragma region Distortion
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Distortion"), CUI_Distortion::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Distortion"))));
#pragma endregion End

#pragma region Crosshair
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MouseCursor"), CUI_MouseCursor::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_MouseCursor"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_AimCrosshair"), CUI_AimCrosshair::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_AimCrosshair"))));
#pragma endregion End


#pragma region Loading
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BackGround"), CUI_BackGround::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_BackGround"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading_Icon"), CUI_Loading_Icon::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Loading_Icon"))));
#pragma endregion End

#pragma region Video
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Video"), CUI_Video::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Video"))));
	// MainMenu
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Sprite"), CUI_Sprite::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Sprite"))));
#pragma endregion End
	
#pragma region Interaction
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Interaction"), CUI_Interaction::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Interaction"))));
#pragma endregion End
#pragma region MainMenu
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MainMenuList"), CUI_MainMenuList::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_MainMenuList"))));
#pragma endregion End

	return S_OK;
}

HRESULT CLoader::Ready_Environment_Model(LEVEL eLevel)
   {
	lstrcpy(m_szLoadingText, TEXT("환경모델을 로드하는 중입니다."));

	if (eLevel == LEVEL_GAMEPLAY)
	{
		wstring					strNonAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage1/NonAnim/");
		wstring					strAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage1/Anim/");

		//! 로더에 원형
		FAILED_CHECK(Read_FBXModelPath(strNonAnimModelPath.c_str(), eLevel, CModel::TYPE_NONANIM));
		FAILED_CHECK(Read_FBXModelPath(strAnimModelPath.c_str(), eLevel, CModel::TYPE_ANIM));
	}
	else if (eLevel == LEVEL_INTRO_BOSS || eLevel == LEVEL_INTRO)
	{
		wstring					strNonAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage1BossMap/NonAnim/");
		//wstring					strAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage1BossMap/Anim/");

		//! 로더에 원형
		FAILED_CHECK(Read_FBXModelPath(strNonAnimModelPath.c_str(), eLevel, CModel::TYPE_NONANIM));
		//FAILED_CHECK(Read_FBXModelPath(strAnimModelPath.c_str(), eLevel, CModel::TYPE_ANIM));
	}
	else if (eLevel == LEVEL_SNOWMOUNTAIN)
	{
		wstring					strNonAnimModelPath = TEXT("../Bin/Resources/Models/Map/SnowMountain/NonAnim/");
		wstring					strAnimModelPath = TEXT("../Bin/Resources/Models/Map/SnowMountain/Anim/");

		FAILED_CHECK(Read_FBXModelPath(strNonAnimModelPath.c_str(), eLevel, CModel::TYPE_NONANIM));
		FAILED_CHECK(Read_FBXModelPath(strAnimModelPath.c_str(), eLevel, CModel::TYPE_ANIM));
	}
	else if (eLevel == LEVEL_SNOWMOUNTAINBOSS)
	{
		wstring					strNonAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage2BossTestMap/NonAnim/");
		wstring					strAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage2BossTestMap/Anim/");

		FAILED_CHECK(Read_FBXModelPath(strNonAnimModelPath.c_str(), eLevel, CModel::TYPE_NONANIM));
		FAILED_CHECK(Read_FBXModelPath(strAnimModelPath.c_str(), eLevel, CModel::TYPE_ANIM));
	}
	else if (eLevel == LEVEL_TOOL)
	{
		//wstring					strNonAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage1/NonAnim/");
		wstring				strNonAnimModelPath = TEXT("../Bin/Resources/Models/Map/SnowMounTain/NonAnim/");
		//wstring				strNonAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage1BossMap/NonAnim/");
		//wstring				strNonAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage2BossTestMap/NonAnim/");

		//! 로더에 원형
		FAILED_CHECK(Read_FBXModelPath(strNonAnimModelPath.c_str(), eLevel, CModel::TYPE_NONANIM));

		//wstring					strAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage1/Anim/");
		wstring				strAnimModelPath = TEXT("../Bin/Resources/Models/Map/SnowMounTain/Anim/");
		//wstring				strNonAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage2BossTestMap/Anim/");
		
		FAILED_CHECK(Read_FBXModelPath(strAnimModelPath.c_str(), eLevel, CModel::TYPE_ANIM));
	}

	return S_OK;
}

HRESULT CLoader::Ready_UI_Tool_Texture(LEVEL eLevel)
{
	/* !UI : MainMenu */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("MainMenu_Start"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Video/MainMenu/Start/MainMenu_Start (%d).dds"), 729)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("MainMenu_Loop"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Video/MainMenu/Loop/MainMenu_Loop (%d).dds"), 341)));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("MainMenu_GamePlay"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Textures/MainMenu/MainMenu_GamePlay.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("MainMenu_Intro"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Textures/MainMenu/MainMenu_Intro.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("MainMenu_IntroBoss"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Textures/MainMenu/MainMenu_IntroBoss.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("MainMenu_SnowMountain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Textures/MainMenu/MainMenu_SnowMountain.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("MainMenu_SnowMountainBoss"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Textures/MainMenu/MainMenu_SnowMountainBoss.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("MainMenu_Tool"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Textures/MainMenu/MainMenu_Tool.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("MainMenu_Exit"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Textures/MainMenu/MainMenu_Exit.dds"))));

	return S_OK;
}

HRESULT CLoader::Read_FBXModelPath(const _tchar* StartDirectoryPath, LEVEL eLevel, _int iAnimType) //! 승용
{
	
	//! C++ 17부터 지원하는 filesystem을 이용해서 특정 경로안에 하위경로들을 전부 탐색 하여 fbx확장자들을 찾아준다
	//! 

	namespace fs = std::filesystem; 

	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	
	map<wstring,CImgui_Manager::JSY_MODEL_TYPE>* pMapModelTag;

	//vector<wstring>* pVecModelTag; //! Imgui_Manager의 모델태그 벡터 받기 위함.

	if (iAnimType == ECast(CModel::TYPE::TYPE_ANIM))
		pMapModelTag = CImgui_Manager::GetInstance()->Get_Anim_E_ModelTag();
	else
		pMapModelTag = CImgui_Manager::GetInstance()->Get_NonAnim_E_ModelTag();


		
	//! 폴더명으로 타입을 분류하기위해
		wstring strDirName = {}; 
		CImgui_Manager::JSY_MODEL_TYPE eModelType = CImgui_Manager::JSY_MODEL_TYPE::MODEL_END;

		for (const auto& entry : fs::recursive_directory_iterator(StartDirectoryPath)) 
	{
		if (fs::is_directory(entry.path())) 
		{
			strDirName = entry.path().filename();

			if (strDirName == L"Instance")
				eModelType = CImgui_Manager::JSY_MODEL_TYPE::MODEL_INSTANCE;
			else if (strDirName == L"Interact")
				eModelType = CImgui_Manager::JSY_MODEL_TYPE::MODEL_INTERACT;
			else if (strDirName == L"Single")
				eModelType = CImgui_Manager::JSY_MODEL_TYPE::MODEL_SINGLE;
		}

		 if (fs::is_regular_file(entry.path()) && entry.path().extension().string() == ".fbx")
		{
			wstring strSearchPath = entry.path().wstring();
			
			fs::path PathObj(strSearchPath);


			wstring wstrFileName = PathObj.stem().wstring();
			wstring wstrFBXPath = PathObj.parent_path() / wstrFileName;

			wstring wstrSliceModelTag = L"Prototype_Component_Model_" + wstrFileName; //! 모델 태그 만들기

			string strConvertFBXPath;
			m_pGameInstance->WString_To_String(wstrFBXPath, strConvertFBXPath);
			
			//if (strConvertFBXPath == "../Bin/Resources/Models/Map/Road\\Track\\Track_Switch_Module20m\\Track_Module20m") 디버깅용
			//{
			//	_int i = 0;
			//}

			FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, wstrSliceModelTag, CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)iAnimType, strConvertFBXPath, PivotMatrix)));

			pMapModelTag->emplace(wstrSliceModelTag, eModelType);
		}
	}

 	return S_OK;
}

HRESULT CLoader::Read_FBXModelPath_NoScale(const _tchar* StartDirectoryPath, LEVEL eLevel, _int iAnimType)
{
	//! C++ 17부터 지원하는 filesystem을 이용해서 특정 경로안에 하위경로들을 전부 탐색 하여 fbx확장자들을 찾아준다
	//! 

	namespace fs = std::filesystem;

	_matrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	map<wstring, CImgui_Manager::JSY_MODEL_TYPE>* pMapModelTag;

	//vector<wstring>* pVecModelTag; //! Imgui_Manager의 모델태그 벡터 받기 위함.

	if (iAnimType == ECast(CModel::TYPE::TYPE_ANIM))
		pMapModelTag = CImgui_Manager::GetInstance()->Get_Anim_E_ModelTag();
	else
		pMapModelTag = CImgui_Manager::GetInstance()->Get_NonAnim_E_ModelTag();



	//! 폴더명으로 타입을 분류하기위해
	wstring strDirName = {};
	CImgui_Manager::JSY_MODEL_TYPE eModelType = CImgui_Manager::JSY_MODEL_TYPE::MODEL_END;

	for (const auto& entry : fs::recursive_directory_iterator(StartDirectoryPath))
	{
		if (fs::is_directory(entry.path()))
		{
			strDirName = entry.path().filename();

			if (strDirName == L"Instance")
				eModelType = CImgui_Manager::JSY_MODEL_TYPE::MODEL_INSTANCE;
			else if (strDirName == L"Interact")
				eModelType = CImgui_Manager::JSY_MODEL_TYPE::MODEL_INTERACT;
			else if (strDirName == L"Single")
				eModelType = CImgui_Manager::JSY_MODEL_TYPE::MODEL_SINGLE;
		}

		if (fs::is_regular_file(entry.path()) && entry.path().extension().string() == ".fbx")
		{
			wstring strSearchPath = entry.path().wstring();

			fs::path PathObj(strSearchPath);


			wstring wstrFileName = PathObj.stem().wstring();
			wstring wstrFBXPath = PathObj.parent_path() / wstrFileName;

			wstring wstrSliceModelTag = L"Prototype_Component_Model_" + wstrFileName; //! 모델 태그 만들기

			string strConvertFBXPath;
			m_pGameInstance->WString_To_String(wstrFBXPath, strConvertFBXPath);

			//if (strConvertFBXPath == "../Bin/Resources/Models/Map/Road\\Track\\Track_Switch_Module20m\\Track_Module20m") 디버깅용
			//{
			//	_int i = 0;
			//}

			FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, wstrSliceModelTag, CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)iAnimType, strConvertFBXPath, PivotMatrix)));

			pMapModelTag->emplace(wstrSliceModelTag, eModelType);
		}
	}

	return S_OK;
}

HRESULT CLoader::Ready_Effect_Origin()
{
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Particle"), CEffect_Particle::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Particle"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Instance"), CEffect_Instance::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Instance"))));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"), CEffect::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect"))));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Trail"), CEffect_Trail::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Trail"))));


	//! 안 쓸 예정(아직은 있어야함)
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Rect"), CEffect_Rect::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Rect"))));

	return S_OK;
}

HRESULT CLoader::Ready_Effect_All(LEVEL eLevel)
{
	//! 이펙트 텍스처 로드
	FAILED_CHECK(Ready_Effect_Texture(eLevel));

	//! 이펙트 모델 로드
	FAILED_CHECK(Ready_Effect_Model(eLevel));

	//! 이펙트 버퍼 로드
	FAILED_CHECK(Ready_Effect_Buffer(eLevel))


	return S_OK;
}

HRESULT CLoader::Ready_Effect_Texture(LEVEL eLevel)
{
	/* Diffuse */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Effect_Diffuse"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/00_Diffuse/Diffuse_%d.dds"), 24)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Effect_Diffuse_Waves"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/00_Diffuse/Waves/Diffuse_Wave_%d.dds"), 6)));

	/* Normal */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Effect_Normal"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/01_Normal/Normal_%d.dds"), 1)));


	/* Sprite */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Effect_Diffuse_Sprite"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/04_Sprites/Sprite_%d.dds"), 25)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Effect_Diffuse_Sprite_Blood"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/04_Sprites/Blood/Sprite_blood_%d.dds"), 29)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Effect_Diffuse_Sprite_Smokes"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/04_Sprites/Smokes/Sprite_smoke_%d.dds"), 35)));


	return S_OK;
}

HRESULT CLoader::Ready_Effect_Model(LEVEL eLevel)
{
	_matrix		PivotMatrix = {};
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));


	//! 스태틱 이펙트 모델
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Aoe_Lens"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Aoe_Lens/Aoe_Lens", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Billboard_Circle_00"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Billboard_Circle_00/Billboard_Circle_00", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Ring"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Ring/Ring", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Sphere"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Sphere/Sphere", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Projectile"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Projectile/Projectile", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Corn"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Corn/Corn", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_ShieldDome"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/ShieldDome/ShieldDome", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Tornado"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Tornado/Tornado", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Tornado_cream"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Tornado_cream/Tornado_cream", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Tornado_splineMesh"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Tornado_splineMesh/Tornado_splineMesh", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_WinchesterElectric"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/WinchesterElectric/WinchesterElectric", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_LightningFast"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/LightningFast/LightningFast", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_LightningsPack"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/LightningsPack/LightningsPack", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_HemiSphere"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/HemiSphere/HemiSphere", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Slash_00"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Slash_00/Slash_00", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Slash_01"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Slash_01/Slash_01", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Coil"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Coil/Coil", PivotMatrix)));


	//! 파티클 이펙트 모델
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Rock_00"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Particle_Mesh/Rock_00/Rock_00", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Rock_01"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Particle_Mesh/Rock_01/Rock_01", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Rock_02"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Particle_Mesh/Rock_02/Rock_02", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Rock_03"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Particle_Mesh/Rock_03/Rock_03", PivotMatrix)));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_LightningParticle_00"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Particle_Mesh/LightningParticle_00/LightningParticle_00", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_LightningParticle_01"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Particle_Mesh/LightningParticle_01/LightningParticle_01", PivotMatrix)));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_LeafPlane"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Particle_Mesh/LeafPlane/LeafPlane", PivotMatrix)));


	//! 일반 몬스터용 이펙트 모델
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Bioball_00"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Bioball_00/Bioball_00", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Bioball_01"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Bioball_01/Bioball_01", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Bioball_02"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Static_Mesh/Bioball_02/Bioball_02", PivotMatrix)));


	//! 인트로보스용 이펙트 모델
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_BatStorm_01"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/VampireCommander/BatStorm/BatStorm_01/BatStorm_01", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_BatStorm_02"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/VampireCommander/BatStorm/BatStorm_02/BatStorm_02", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_BeastSkull"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/VampireCommander/BeastSkull/BeastSkull", PivotMatrix)));
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);	// Impact 모델은 180도 돌리면 안됨
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Impact"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/VampireCommander/Impact/Impact", PivotMatrix)));


	//! 두번째 보스용 이펙트 모델
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Egg"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Parasiter/Egg/Egg", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Egg_Mother"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Parasiter/Egg_Mother/Egg_Mother", PivotMatrix)));



	return S_OK;
}

HRESULT CLoader::Ready_Effect_Buffer(LEVEL eLevel)
{
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Trail"), CVIBuffer_Trail::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Effect_Model_Instance"), CVIBuffer_Effect_Model_Instance::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Particle"), CVIBuffer_Particle::Create(m_pDevice, m_pContext)));

	// Rect는 Static

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_CriticalSection);
	DeleteObject(m_hThread);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
