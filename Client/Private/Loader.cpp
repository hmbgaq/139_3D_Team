#include "stdafx.h"
#include "Loader.h"
#include "GameInstance.h"
#include <process.h>

#pragma region GAMEOBJECT
#include "Camera_Dynamic.h"
#include "Weapon_Player.h"
#include "Body_Player.h"
#include "BackGround.h"
#include "Field.h"
#include "ForkLift.h"
#include "Terrain.h"
#include "Monster.h"
#include "Player.h"
#include "Sky.h"
#include "SkyDome.h"
#include "Environment_Object.h"
#include "Environment_Instance.h"
#include "Screamer.h"
#pragma endregion

#pragma region UI
#include "UI_MonsterHpFrame.h"
#include "UI_MonsterHp.h"
#include "UI_Anything.h"
#pragma endregion

#pragma region Test
#include "Interact_Chain.h"
#pragma endregion

#pragma region Effect
#include "Effect.h"
#include "Effect_Particle.h"
#include "Effect_Rect.h"
#include "Effect_Instance.h"
//#include "Effect_Mesh.h"
//#include "Effect_Trail.h"
//#include "Effect_Decal.h"

/* Effect_Legacy */
#include "Effect_Explosion.h"
#include "Particle_Blue.h"
#include "Particle_Red.h"
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
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_Dissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/dissolve_tex.png"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_DownTop_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/DownTopDask.png"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Texture_BreakMask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/BreakMask.dds"))));

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));

	//! 캐릭터 모델
	_matrix      PivotMatrix;
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Fiona"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Screamer"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Screamer/Screamer", PivotMatrix)));

	/* 1/100 스케일 적용 */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_ForkLift"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Model_Chain"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Chain/Chain", PivotMatrix)));

	//!환경 모델
	//!		

	//Ready_Environment_Model(LEVEL_GAMEPLAY);


	//!버퍼 
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Particle_Rect"), CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, 100)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Particle_Point"), CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, 500)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Environment_Model_Instance"), CVIBuffer_Environment_Model_Instance::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_VIBuffer_Effect_Model_Instance"), CVIBuffer_Effect_Model_Instance::Create(m_pDevice, m_pContext)));

	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_Model"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_AnimModel"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_VtxCube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_Particle_Rect"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Rect.hlsl"), VTX_PARTICLE_RECT::Elements, VTX_PARTICLE_RECT::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_Particle_Point"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Point.hlsl"), VTX_PARTICLE_POINT::Elements, VTX_PARTICLE_POINT::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_Model_Instance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModelInstance.hlsl"), VTXMODEL_INSTANCE_DECLARATION::Elements, VTXMODEL_INSTANCE_DECLARATION::iNumElements)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Shader_Effect_Model_Instance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectModelInstance.hlsl"), VTXMODEL_INSTANCE_DECLARATION::Elements, VTXMODEL_INSTANCE_DECLARATION::iNumElements)));

	lstrcpy(m_szLoadingText, TEXT("네비게이션를(을) 로드하는 중입니다."));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Navigation2"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation2.dat"))));

	lstrcpy(m_szLoadingText, TEXT("콜라이더를(을) 로드하는 중입니다."));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLEVEL, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE)));

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
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Effect_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Mask/Mask_%d.dds"), 23)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Effect_Noise"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Noise/Noise_%d.dds"), 9)));


	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));
	_matrix		PivotMatrix;

	//! 캐릭터 모델
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Fiona"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona", PivotMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Fiona"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player", PivotMatrix)));

	
	
	//! 환경 모델
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//Ready_Environment_Model(LEVEL_TOOL);

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_ForkLift"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Chain"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Chain/Chain", PivotMatrix)));

	//! 이펙트 모델
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Xray_ManHeavy"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Xray/Xray_ManHeavy", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Mesh_Fire_Test"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/Mesh_Fire_Test/Mesh_Fire_Test", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_ShieldDome"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/00TEST/Weapon/Zapper/ShieldDome", PivotMatrix)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_splineMesh_tornado"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect_Mesh/splineMesh_tornado/splineMesh_tornado", PivotMatrix)));

	//!버퍼 
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Field"), CVIBuffer_Field::Create(m_pDevice, m_pContext))); //! 툴용 버퍼
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Particle_Rect"), CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, 100)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Particle_Point"), CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, 500)));
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

	lstrcpy(m_szLoadingText, TEXT("원형객체를(을) 로드하는 중입니다."));
	
	

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Ready_Origin()
{
	/* Logo 원형객체 */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_BackGround"))));\
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Field"), CField::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Field"))));

	/* GamePlay 원형객체 */
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"), CBody_Player::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Body_Player"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Player"), CWeapon_Player::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Weapon_Player"))));
	
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Terrain"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"), CMonster::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Monster"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"), CForkLift::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_ForkLift"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Screamer"), CScreamer::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Screamer"))));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"), CCamera_Dynamic::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Camera_Dynamic"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Sky"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Blue"), CParticle_Blue::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Particle_Blue"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Red"), CParticle_Red::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Particle_Red"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Explosion"), CEffect_Explosion::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Explosion"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Instance"), CEffect_Instance::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Instance"))));

	//! 상호작용 없는 환경
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_Instance"), CEnvironment_Instance::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Environment_Instance"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_Object"), CEnvironment_Object::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Environment_Object"))));
	


	//! 상호작용 환경
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Interact_Chain"), CInteract_Chain::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Interact_Chain"))));

#pragma region UI
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHpFrame"), CUI_MonsterHpFrame::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_MonsterHpFrame"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHp"), CUI_MonsterHp::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_MonsterHp"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Anything"), CUI_Anything::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_UI_Anything"))));
#pragma endregion

#pragma region Effect
	//!유정
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"), CEffect::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Particle"), CEffect_Particle::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Particle"))));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Rect"), CEffect_Rect::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Effect_Rect"))));
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Ready_Environment_Model(LEVEL eLevel)
{

	wstring					strNonAnimModelPath = TEXT("../Bin/Resources/Models/Map/NonAnim/");
	wstring					strAnimModelPath = TEXT("../Bin/Resources/Models/Map/Anim/");

	//! 로더에 원형
	FAILED_CHECK(Read_FBXModelPath(strNonAnimModelPath.c_str(), eLevel, CModel::TYPE_NONANIM));
	//
	FAILED_CHECK(Read_FBXModelPath(strAnimModelPath.c_str(), eLevel, CModel::TYPE_ANIM));


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

	if (iAnimType == CModel::TYPE::TYPE_ANIM)
		pMapModelTag = CImgui_Manager::GetInstance()->Get_Anim_E_ModelTag();
	else
		pMapModelTag = CImgui_Manager::GetInstance()->Get_NonAnim_E_ModelTag();

	//! 폴더명으로 타입을 분류하기위해
		wstring strDirName = {}; 
		CImgui_Manager::JSY_MODEL_TYPE eModelType = CImgui_Manager::JSY_MODEL_TYPE::MODEL_END;

	for (const auto& entry : fs::recursive_directory_iterator(StartDirectoryPath)) 
	{
		if (fs::is_directory(entry.path())) {
			strDirName = entry.path().filename();

			if (strDirName == L"Environment")
				eModelType = CImgui_Manager::JSY_MODEL_TYPE::MODEL_ENVIRONMENT;
			else if (strDirName == L"Interact")
				eModelType = CImgui_Manager::JSY_MODEL_TYPE::MODEL_INTERACT;
			else if (strDirName == L"Ground")
				eModelType = CImgui_Manager::JSY_MODEL_TYPE::MODEL_GROUND;
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
