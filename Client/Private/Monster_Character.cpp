#include "Monster_Character.h"
#include "Transform.h"
#include "Navigation.h"

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
				m_pNavigationCom->Set_CurrentIndex(m_pNavigationCom->Get_SelectRangeCellIndex(this));
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
}

HRESULT CMonster_Character::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}


void CMonster_Character::Search_Target()
{
	__super::Search_Target(LAYER_PLAYER);
}

void CMonster_Character::Free()
{
	__super::Free();
}
