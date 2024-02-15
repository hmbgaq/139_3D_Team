#include "stdafx.h"
#include "Loader.h"
#include "GameInstance.h"
#include <process.h>

#pragma region GAMEOBJECT
#include "Effect_Explosion.h"
#include "Camera_Dynamic.h"
#include "Weapon_Player.h"
#include "Particle_Blue.h"
#include "Particle_Red.h"
#include "Body_Player.h"
#include "BackGround.h"
#include "ForkLift.h"
#include "Terrain.h"
#include "Monster.h"
#include "Player.h"
#include "Sky.h"
#include "SkyDome.h"
#include "Effect_Instance.h"
#include "Environment_Instance.h"
#pragma endregion

#pragma region Test
#include "Interact_Chain.h"
#pragma endregion

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

	/* For.Prototype_Component_Texture_Logo */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2)));


	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));


	lstrcpy(m_szLoadingText, TEXT("원형객체를(을) 로드하는 중입니다."));

	Ready_Origin();

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level()
{
	/* 게임플레이 레벨에 필요한 자원을 로드하자. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/MyMask.dds"), 1)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Brush"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/dissolve_tex.png"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DownTop_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/DownTopDask.png"))));

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));
	_matrix		PivotMatrix;
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona", PivotMatrix)));

	/* 1/100 스케일 적용 */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Chain"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Chain/Chain", PivotMatrix)));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rect"), CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, 100)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Point"), CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, 100)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Environment_Model_Instance"), CVIBuffer_Environment_Model_Instance::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Effect_Model_Instance"), CVIBuffer_Effect_Model_Instance::Create(m_pDevice, m_pContext)));

	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Particle_Rect"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Rect.hlsl"), VTX_PARTICLE_RECT::Elements, VTX_PARTICLE_RECT::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Particle_Point"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Point.hlsl"), VTX_PARTICLE_POINT::Elements, VTX_PARTICLE_POINT::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model_Instance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModelInstance.hlsl"), VTXMODEL_INSTANCE_DECLARATION::Elements, VTXMODEL_INSTANCE_DECLARATION::iNumElements)));

	lstrcpy(m_szLoadingText, TEXT("네비게이션를(을) 로드하는 중입니다."));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation2"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation2.dat"))));

	lstrcpy(m_szLoadingText, TEXT("콜리이더를(을) 로드하는 중입니다."));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE)));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Tool_Level()
{
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Ready_Origin()
{
	/* Logo 원형객체 */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext)));

	/* GamePlay 원형객체 */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"), CBody_Player::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Player"), CWeapon_Player::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"), CMonster::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"), CForkLift::Create(m_pDevice, m_pContext)));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"), CCamera_Dynamic::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Blue"), CParticle_Blue::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Red"), CParticle_Red::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Explosion"), CEffect_Explosion::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Instance"), CEffect_Instance::Create(m_pDevice, m_pContext)));

	//! 상호작용 없는 환경
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_Instance"), CEnvironment_Instance::Create(m_pDevice, m_pContext)));

	//! 상호작용 환경
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Interact_Chain"), CInteract_Chain::Create(m_pDevice, m_pContext)));


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
