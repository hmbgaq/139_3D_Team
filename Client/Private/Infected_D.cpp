#include "SMath.h"
#include "stdafx.h"
#include "Infected_D.h"
#include "GameInstance.h"
#include "Data_Manager.h"

/* Spawn */
#include "Infected_IdleAct_01.h"

CInfected_D::CInfected_D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CInfected(pDevice, pContext, strPrototypeTag)
{
}

CInfected_D::CInfected_D(const CInfected_D& rhs)
	: CInfected(rhs)
{
}

HRESULT CInfected_D::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CInfected_D::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(Ready_Option());

	/* !성희 추가 : 몬스터 HUD 생성 */
	Ready_EnemyHUD_Shard(m_pGameInstance->Get_NextLevel(), this);

	return S_OK;
}

void CInfected_D::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CInfected_D::Tick(_float fTimeDelta)
{
	if (GAME_STATE::GAMEPLAY != m_pDataManager->Get_GameState())
		return;

	__super::Tick(fTimeDelta);


}

void CInfected_D::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_OUTLINE, this), ); //m_bIsInFrustum
}

HRESULT CInfected_D::Render()
{
	return S_OK;
}

HRESULT CInfected_D::Render_Shadow()
{
	return S_OK;
}

HRESULT CInfected_D::Render_OutLine()
{
	return S_OK;
}

HRESULT CInfected_D::Update_RenderState(_int CurrentHP)
{
	return S_OK;
}

void CInfected_D::Explosion()
{
	CGameObject* pBullet = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_MONSTER_BULLET, L"Prototype_GameObject_Explosion_Infected_D");
	_float3 vSpawnPos = Get_Position();
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	pBullet->Set_Position(vSpawnPos);
	pBullet->Get_Transform()->Look_At(vLook * -1);
}

#pragma region Base Setting

HRESULT CInfected_D::Ready_Components()
{
	FAILED_CHECK(__super::Ready_Components());

	return S_OK;
}

HRESULT CInfected_D::Ready_Option()
{
	// 0		~	fAttack : 일반공격 거리 - 공격이후에는 쿨타임이라 뒷걸음질치는거 
	// fAttack	~	fWalk	: 걸어오는 거리 
	// fWalk	~			: 뛰어오는거리 -> 일반공격에 도달할때까지 뛰어옴 -> 뛰면서 공격하는 이동공격으로 감 
	m_eInfo.eType = INFECTED_TYPE::INFECTED_WASTER;
	m_eInfo.RandomNumber = SMath::Random(1, 10);
	m_eInfo.fAttack_Distance = 1.5f; /* 이 사거리 내에 있으면 무조건 자폭 */
	m_eInfo.fWalk_Distance = 1.5f; 

	m_fHp = 10;
	m_fMaxHp = 10;

	m_pTarget = m_pGameInstance->Get_Player();

	/* 액터 할당 & 리스폰 애니메이션 지정 */
	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CInfected>(this);
		m_pActor->Set_State(new CInfected_IdleAct_01());
	}

	return S_OK;
}

void CInfected_D::Check_Frustum()
{
	m_bIsInFrustum = true;
}

HRESULT CInfected_D::Ready_PartObjects()
{
	/* For. Body */
	{
		CBody::BODY_DESC		BodyDesc = {};
		FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Infected_D"), BodyDesc));
	}

	/* For. Weapon */
	{
		CWeapon::WEAPON_DESC		WeaponDesc = {};
		WeaponDesc.m_pSocketBone = m_pBody->Get_BonePtr("HipsStretch");
		WeaponDesc.m_pParentTransform = m_pTransformCom;
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Infected_D"), "HipsStretch", WeaponDesc, TEXT("Weapon_Bomb")));
	
		CWeapon* m_pWeapon = Get_Weapon(TEXT("Weapon_Bomb"));
		m_pWeapon->Set_Enable(false);
	}
	return S_OK;
}

#pragma endregion


#pragma region Create, Clone, Free, Pool

CInfected_D* CInfected_D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CInfected_D* pInstance = new CInfected_D(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInfected_D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInfected_D::Clone(void* pArg)
{
	CInfected_D* pInstance = new CInfected_D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInfected_D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInfected_D::Pool()
{
	return new CInfected_D(*this);
}

void CInfected_D::Free()
{
	__super::Free();
}

#pragma endregion
