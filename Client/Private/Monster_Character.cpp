#include "Monster_Character.h"
#include "Transform.h"
#include "Navigation.h"
#include "Player.h"
#include "Data_Manager.h"
#include "Transform.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "UI_EnemyHUD_Shard.h"

CMonster_Character::CMonster_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CCharacter_Client(pDevice, pContext, strPrototypeTag)
{
}

CMonster_Character::CMonster_Character(const CMonster_Character& rhs)
	: CCharacter_Client(rhs)
{
}

HRESULT CMonster_Character::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CMonster_Character::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	_bool bMonsterDesc = false;


	if (pArg != nullptr)
	{
		CGameObject::GAMEOBJECT_DESC ObjectDesc = *(CGameObject::GAMEOBJECT_DESC*)pArg;
		
		if(ObjectDesc.eDescType == CGameObject::MONSTER_DESC)
			bMonsterDesc = true;


		if (bMonsterDesc == true)
		{
			m_tMonsterDesc = *(MONSTER_DESC*)pArg;

			if (m_tMonsterDesc.bPreview == false)
			{
				m_pTransformCom->Set_WorldMatrix(m_tMonsterDesc.WorldMatrix);
				m_pNavigationCom->Set_CurrentIndex(m_tMonsterDesc.iStartNaviIndex);
				//m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				//m_pNavigationCom->Set_CurrentIndex(m_pNavigationCom->Get_SelectRangeCellIndex(this));
			}
		}
	}

	

	return S_OK;
}

void CMonster_Character::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CMonster_Character::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMonster_Character::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (true == m_bDead || false == m_bEnable)
	{
		Set_EnemyHUD_Dead();
	}
}

HRESULT CMonster_Character::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}


void CMonster_Character::Search_Target(const _float fSearchDistance)
{
	__super::Search_Target(LAYER_PLAYER, fSearchDistance);
}

CPlayer* CMonster_Character::Set_Player_Finisher_Pos(_float3 vPlayerPos)
{
	m_pBody->Collider_Off();

	CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();
	_float3 vPos = m_pTransformCom->Calc_Front_Pos(vPlayerPos);
	pPlayer->Set_Position(vPos);
	pPlayer->Get_Transform()->Look_At(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));

	return pPlayer;
}

CPlayer* CMonster_Character::Set_Finish_Pos(_float3 vPos)
{
	m_pBody->Collider_Off();

	CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();

	_float3 vTargetPos = pPlayer->Calc_Front_Pos(vPos);
	Set_Position(vTargetPos);
	m_pTransformCom->Look_At(pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION));

	return pPlayer;
}

void CMonster_Character::Check_Frustum()
{
	m_bIsInFrustum = m_pGameInstance->isIn_WorldPlanes(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
}

// 몬스터 HUD를 준비합니다. (생성 : Level, Owner)
void CMonster_Character::Ready_EnemyHUD_Shard(_uint iLevelIndex, CGameObject* pOwner)
{
	m_pEnemyHUD = m_pUIManager->Ready_EnemyHUD_Shard(iLevelIndex, pOwner);
}

// 몬스터 HUD의 위치를 잡아줍니다. (매 틱마다 불러주세요. | 몬스터의 World매트릭스를 넣어주세요. | Offset만큼 위치를 움직일 수 있습니다. [Defualt (오프셋 안줬을 경우) : 0, 2, 0]
void CMonster_Character::Check_EnemyHUD_World(_matrix matWorld, _float3 vOffsetPos)
{
	if (m_pEnemyHUD != nullptr)
		m_pEnemyHUD->Set_EnemyHUD_World(matWorld, vOffsetPos);
}

// 몬스터 HUD를 삭제(비활성화)합니다. (몬스터가 죽을때 불러주세요)
void CMonster_Character::Set_EnemyHUD_Dead()
{
	if (m_pEnemyHUD != nullptr) 
	{
		m_pEnemyHUD->Set_Dead_Owner(true);
		m_pEnemyHUD = nullptr;
	}
		
}

void CMonster_Character::Free()
{
	__super::Free();
}
