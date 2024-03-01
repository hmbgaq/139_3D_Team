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
#include "Environment_Instance.h"
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
#pragma endregion

#pragma region INFECTED
#include "Infected.h"
#include "Body_Infected.h"
#pragma endregion

#pragma region ASSASSIN
#include "Assassin.h"
#include "Body_Assassin.h"
#pragma endregion

#pragma region BANDIT_HEAVY
#include "Bandit_Heavy.h"
#include "Body_Bandit_Heavy.h"
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
#include "UI_Blood.h"
#include "UI_Blood_Vignette.h"
#include "UI_Blood_Veins.h"
#include "UI_Blood_Lights.h"
/* Monster */
#include "UI_MonsterHpFrame.h"
#include "UI_MonsterHp.h"
#pragma endregion

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

/* Effect_Legacy */
#include "Effect_Explosion.h"
#include "Particle_Blue.h"
#include "Particle_Red.h"
#pragma endregion

#pragma region PhysX
#include "PhysXCollider.h"
#include "PhysXController.h"
#include "PhysXCharacterController.h"

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

	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlay_Level();
		break;

	case LEVEL_SNOWMOUNTAIN:
		hr = Loading_For_SnowMountain_Level();
		break;

	case LEVEL_LAVA:
		hr = Loading_For_Lava_Level();
		break;

	case LEVEL_TOOL:
		hr = Loading_For_Tool_Level();
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
	/* 게임플레이 레벨에 필요한 자원을 로드하자. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));


	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Terrain_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/MyMask.dds"), 1)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Terrain_Brush"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Snow"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Explosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Dissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/dissolve_tex.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_DownTop_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/DownTopDask.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_BreakMask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/BreakMask.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Effect_Particle_Base"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Particle/Diffuse/Base/Base_%d.dds"), 14)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Effect_Particle_Blood"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Particle/Diffuse/Blood/Blood_%d.dds"), 1)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Effect_Sprite"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Diffuse/Sprites/Sprite_%d.dds"), 17)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Effect_Diffuse"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Diffuse/Diffuse_%d.dds"), 5)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Effect_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Mask/Mask_%d.dds"), 18)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Effect_Noise"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Noise/Noise_%d.dds"), 6)));


	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));

	//! 캐릭터 모델
	_matrix      PivotMatrix;
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Fiona"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/A/CharacterBase_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/B/CharacterBase_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/C/CharacterBase_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/D/CharacterBase_Skeleton", PivotMatrix)));
	//! 터짐 텍스처 찾아야됨 FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_E"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/E/CharacterBase_Skeleton", PivotMatrix)));


	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Rentier"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player", PivotMatrix)));

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Screamer"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Screamer/Screamer", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Fiona"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Fiona"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player", PivotMatrix)));
	
	
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Assassin"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Assassin/ReverseFootBase_Skeleton", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/A/CharacterBase_Skeleton", PivotMatrix)));


	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_BeastBoss_Phase1"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Beast/Phase1/Beast_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_BeastBoss_Phase2"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Beast/Phase2/Beast_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_BeastBoss_Phase3"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Beast/Phase3/Beast_Skeleton", PivotMatrix)));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Bandit_Heavy"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Heavy/ManHeavyBase_Skeleton", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Bandit_Heavy_Vampiric"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Heavy/Bandit_Heavy_Vampiric/ManHeavyBase_Skeleton", PivotMatrix)));
	
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Bandit_Sniper"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Sniper/CharacterBase_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_BooHag"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/BooHag/ReverseFootBase_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Digger"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Digger/ReverseFootBase_Skeleton", PivotMatrix)));
	//
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Gambit"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Gambit/Gambit_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Gambit_Egg"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Gambit/GambitEgg/Gambit_WeakspotEgg_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Gambit_XRay"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Gambit/XRay/Gambit_Skeleton", PivotMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Ranger"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Ranger/Normal/Ranger_Skeleton", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Ranger_Finisher"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Ranger/Finisher/Ranger_Skeleton", PivotMatrix)));
	//
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Stalker"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Stalker/Normal/ReverseFootBase_Skeleton", PivotMatrix)));
	

	//TODO보류 애니메이션 찾아야됨
		//!FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Spider"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Stalker/Spider/StalkerSpider01_Death_01", PivotMatrix)));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_Shotgun"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/Shotgun/Shotgun", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_ELShotgun"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/ELShotgun/ELShotgun", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_ELWinchester"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/ELWinchester/ELWinchester", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_FlameBelcher"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/FlameBelcher/FlameBelcher", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_Winchester"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/Winchester/Winchester", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Player_Weapon_Revolver"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerWeapon/Revolver/PlayerRevolver", PivotMatrix)));

	/* 1/100 스케일 적용 */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_ForkLift"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Chain"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Chain/Chain", PivotMatrix)));

	//!환경 모델
	//!		

	Ready_Environment_Model(eLEVEL);


	//!버퍼 
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Particle_Rect"), CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, 100)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Particle_Point"), CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, 500)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Environment_Model_Instance"), CVIBuffer_Environment_Model_Instance::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Effect_Model_Instance"), CVIBuffer_Effect_Model_Instance::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_AnimModel_Instance"), CVIBuffer_AnimModel_Instance::Create(m_pDevice, m_pContext)));

	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_Model"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_AnimModel"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_VtxCube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_Particle_Rect"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Rect.hlsl"), VTX_PARTICLE_RECT::Elements, VTX_PARTICLE_RECT::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_Particle_Point"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Point.hlsl"), VTX_PARTICLE_POINT::Elements, VTX_PARTICLE_POINT::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_Model_Instance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModelInstance.hlsl"), VTXMODEL_INSTANCE_DECLARATION::Elements, VTXMODEL_INSTANCE_DECLARATION::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_AnimModel_Instance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModelInstance.hlsl"), VTXMODEL_ANIMMODEL_INSTANCE_DECLARATION::Elements, VTXMODEL_ANIMMODEL_INSTANCE_DECLARATION::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_Effect_Model_Instance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectModelInstance.hlsl"), VTXMODEL_INSTANCE_DECLARATION::Elements, VTXMODEL_INSTANCE_DECLARATION::iNumElements)));

	lstrcpy(m_szLoadingText, TEXT("네비게이션를(을) 로드하는 중입니다."));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Navigation2"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation2.dat"))));

	lstrcpy(m_szLoadingText, TEXT("콜라이더를(을) 로드하는 중입니다."));

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
	FAILED_CHECK(Loading_For_GamePlay_Level_Origin(LEVEL_GAMEPLAY));

	return S_OK;
}

HRESULT CLoader::Loading_For_SnowMountain_Level()
{
	FAILED_CHECK(Loading_For_GamePlay_Level_Origin(LEVEL_SNOWMOUNTAIN));

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
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/MyMask.dds"), 1)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain_Brush"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Snow"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Explosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Effect_Particle_Base"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Particle/Diffuse/Base/Base_%d.dds"), 14))); 
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Effect_Particle_Blood"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Particle/Diffuse/Blood/Blood_%d.dds"), 1)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Effect_Sprite"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Diffuse/Sprites/Sprite_%d.dds"), 17)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Effect_Diffuse"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Diffuse/Diffuse_%d.dds"), 15)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Effect_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Mask/Mask_%d.dds"), 18)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Effect_Noise"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Noise/Noise_%d.dds"), 6)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Dissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/dissolve_tex.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_DownTop_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/DownTopDask.dds"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_BreakMask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/BreakMask.dds"))));

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));

	//! Player
	_matrix		PivotMatrix = {};
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Screamer"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Screamer/Screamer", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Fiona"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Rentier"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_Weapon_Shotgun"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/Shotgun/Shotgun", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_Weapon_ELShotgun"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/ELShotgun/ELShotgun", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_Weapon_ELWinchester"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/ELWinchester/ELWinchester", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_Weapon_FlameBelcher"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/FlameBelcher/FlameBelcher", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_Weapon_Winchester"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/Winchester/Winchester", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_Weapon_Revolver"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/PlayerWeapon/Revolver/PlayerRevolver", PivotMatrix)));

	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Infected_A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/A/CharacterBase_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Infected_B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/B/CharacterBase_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Infected_C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/C/CharacterBase_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Infected_D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/D/CharacterBase_Skeleton", PivotMatrix)));
	//!//! 터짐 텍스처 찾아야됨 FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Infected_E"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Infected/E/CharacterBase_Skeleton", PivotMatrix)));
	//!
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Assassin"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Assassin/ReverseFootBase_Skeleton", PivotMatrix)));
	//!
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_BeastBoss_Phase1"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Beast/Phase1/Beast_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_BeastBoss_Phase2"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Beast/Phase2/Beast_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_BeastBoss_Phase3"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Boss/Beast/Phase3/Beast_Skeleton", PivotMatrix)));
	//!
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Bandit_Heavy"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Heavy/ManHeavyBase_Skeleton", PivotMatrix)));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Bandit_Heavy_Vampiric"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Heavy/Bandit_Heavy_Vampiric/ManHeavyBase_Skeleton", PivotMatrix)));
	//!//
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Bandit_Sniper"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bandit_Sniper/CharacterBase_Skeleton", PivotMatrix)));
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
	
	//! 환경 모델
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	Ready_Environment_Model(LEVEL_TOOL);

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_ForkLift"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Chain"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Chain/Chain", PivotMatrix)));

	//! 이펙트 모델
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Xray_ManHeavy"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Xray/Xray_ManHeavy", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Mesh_Fire_Test"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Mesh_Fire_Test/Mesh_Fire_Test", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_ShieldDome"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/00TEST/Weapon/Zapper/ShieldDome", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_splineMesh_tornado"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/splineMesh_tornado/splineMesh_tornado", PivotMatrix)));

	//!버퍼 
	//! 
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Field"), CVIBuffer_Field::Create(m_pDevice, m_pContext))); //! 툴용 버퍼
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Particle_Rect"), CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, 100)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Particle_Point"), CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, 500)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Point"), CVIBuffer_Point::Create(m_pDevice, m_pContext, 500)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Trail"), CVIBuffer_Trail::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Environment_Model_Instance"), CVIBuffer_Environment_Model_Instance::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Effect_Model_Instance"), CVIBuffer_Effect_Model_Instance::Create(m_pDevice, m_pContext)));


	//!셰이더
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Model"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_AnimModel"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxCube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxPosTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_EffectTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_EffectTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Particle_Rect"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Rect.hlsl"), VTX_PARTICLE_RECT::Elements, VTX_PARTICLE_RECT::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Particle_Point"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Point.hlsl"), VTX_PARTICLE_POINT::Elements, VTX_PARTICLE_POINT::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Model_Instance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModelInstance.hlsl"), VTXMODEL_INSTANCE_DECLARATION::Elements, VTXMODEL_INSTANCE_DECLARATION::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Effect_Model_Instance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectModelInstance.hlsl"), VTXMODEL_INSTANCE_DECLARATION::Elements, VTXMODEL_INSTANCE_DECLARATION::iNumElements)));
	
	

	//!내비게이션
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Navigation2"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation2.dat"))));
	
	//!콜라이더
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE)));


	lstrcpy(m_szLoadingText, TEXT("피직스를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_PhysXController"), CPhysXController::Create(m_pDevice, m_pContext)));
	

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_PhysXCharacterController"), CPhysXCharacterController::Create(m_pDevice, m_pContext)));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Ready_Origin()
{
	/* Logo 원형객체 */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_BackGround"))));\
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

	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapon_Punch"), CPlayer_Weapon_Punch::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Weapon_Winchester"))));

	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Player"), CWeapon_Player::Create(m_pDevice, m_pContext)));


	//! Infected
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Infected"), CInfected::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Infected"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Infected"), CBody_Infected::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Infected"))));

	//! Assassin
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Assassin"), CAssassin::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Assassin"), CBody_Assassin::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Assassin"))));

	//! Bandit_Heavy
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bandit_Heavy"), CBandit_Heavy::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Bandit_Heavy"), CBody_Bandit_Heavy::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Assassin"))));



	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Weapon_Winchester"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"), CMonster::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Monster"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"), CForkLift::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_ForkLift"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Screamer"), CScreamer::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Screamer"))));
	//!FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InstanceMonster"), CInstanceMonster::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_InstanceMonster"))));
	

	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"), CCamera_Dynamic::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Camera_Dynamic"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Spring"), CSpringCamera::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Camera_Spring"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MasterCamera"), CMasterCamera::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_MasterCamera"))));


	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Sky"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Blue"), CParticle_Blue::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Particle_Blue"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Red"), CParticle_Red::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Particle_Red"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Explosion"), CEffect_Explosion::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Explosion"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Instance"), CEffect_Instance::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Instance"))));

	//! 상호작용 없는 환경
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_Instance"), CEnvironment_Instance::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Environment_Instance"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_Object"), CEnvironment_Object::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Environment_Object"))));
	
	//!성희
	Ready_UI_Origin();

	// 
	//! 상호작용 환경
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Interact_Chain"), CInteract_Chain::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Interact_Chain"))));


#pragma region Effect
	//!유정
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"), CEffect::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Particle"), CEffect_Particle::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Particle"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Rect"), CEffect_Rect::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Rect"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Trail"), CEffect_Trail::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Trail"))));
#pragma endregion


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

#pragma region Monster
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHpFrame"), CUI_MonsterHpFrame::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_MonsterHpFrame"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHp"), CUI_MonsterHp::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_MonsterHp"))));
#pragma endregion End

	return S_OK;
}

HRESULT CLoader::Ready_Environment_Model(LEVEL eLevel)
{



	if (eLevel == LEVEL_GAMEPLAY)
	{
		wstring					strNonAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage1/NonAnim/");
		wstring					strAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage1/Anim/");

		//! 로더에 원형
		FAILED_CHECK(Read_FBXModelPath(strNonAnimModelPath.c_str(), eLevel, CModel::TYPE_NONANIM));
		FAILED_CHECK(Read_FBXModelPath(strAnimModelPath.c_str(), eLevel, CModel::TYPE_ANIM));
	}
	else if (eLevel == LEVEL_TOOL)
	{
		wstring					strNonAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage1/NonAnim/");
		wstring					strAnimModelPath = TEXT("../Bin/Resources/Models/Map/Stage1/Anim/");

		//! 로더에 원형
		FAILED_CHECK(Read_FBXModelPath(strNonAnimModelPath.c_str(), eLevel, CModel::TYPE_NONANIM));
		FAILED_CHECK(Read_FBXModelPath(strAnimModelPath.c_str(), eLevel, CModel::TYPE_ANIM));

	}


	


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

		 if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".fbx")
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
