#include "stdafx.h"
#include "Player_Weapon_Kick.h"
#include "GameInstance.h"
#include "Character_Client.h"
#include "Player.h"
#include "Data_Manager.h"
#include "MasterCamera.h"
#include "Effect_Manager.h"

CPlayer_Weapon_Kick::CPlayer_Weapon_Kick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CWeapon_Player(pDevice, pContext, strPrototypeTag)
{

}

CPlayer_Weapon_Kick::CPlayer_Weapon_Kick(const CPlayer_Weapon_Kick& rhs)
	: CWeapon_Player(rhs)
{
}

HRESULT CPlayer_Weapon_Kick::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_Kick::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon_Kick::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPlayer_Weapon_Kick::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPlayer_Weapon_Kick::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Weapon_Kick::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_Kick::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();


	m_iColliderSize = 1;
	m_pColliders.resize(m_iColliderSize);

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::PLAYER_ATTACK);
	BoundingDesc.fRadius = { 0.6f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon_Kick::OnCollisionEnter(CCollider* other)
{
	CCharacter* pTarget_Character = Get_Target_Character(other);
	if (nullptr != pTarget_Character)
	{
		_float fDamage = m_fDamage + CData_Manager::GetInstance()->Get_Additional_MeleeDamage();

		_vector vTargetPos = pTarget_Character->Get_Position_Vector();
		pTarget_Character->Set_Hitted(fDamage, Get_Object_Owner()->Calc_Look_Dir_XZ(vTargetPos) * -1, m_fForce, 1.f, m_eHitDirection, m_eHitPower, true);
		//pTarget_Character->Set_Hitted(0, Get_Object_Owner()->Calc_Look_Dir(vTargetPos) * -1, 0.5f, 1.f, Direction::Front, Power::Light);

		//CCharacter_Client* pOwnerCharacter = dynamic_cast<CCharacter_Client*>(Get_Object_Owner());
		//if (pOwnerCharacter)
		//{
		//	pOwnerCharacter->Create_Hitting_Effect(Get_WorldPosition(), m_eHitPower);
		//}


		{
			_float3 vPos = Get_WorldPosition();
			_float3 vTargetPos = pTarget_Character->Get_Position();
			vTargetPos.y = vPos.y;
			EFFECT_MANAGER->Play_Effect("Hit/", "Hit_Distortion.json", nullptr, vPos, true, vTargetPos);

			CData_Manager::GetInstance()->Apply_Shake_And_Blur(m_eHitPower);
		}

		Set_Enable_Collisions(false);
	}
	
}

void CPlayer_Weapon_Kick::OnCollisionStay(CCollider* other)
{
	_int a = 0;
}

void CPlayer_Weapon_Kick::OnCollisionExit(CCollider* other)
{
	_int a = 0;

}

CPlayer_Weapon_Kick* CPlayer_Weapon_Kick::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CPlayer_Weapon_Kick* pInstance = new CPlayer_Weapon_Kick(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Weapon_Kick");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_Kick::Clone(void* pArg)
{
	CPlayer_Weapon_Kick* pInstance = new CPlayer_Weapon_Kick(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Weapon_Kick");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_Kick::Pool()
{
	return new CPlayer_Weapon_Kick(*this);
}

void CPlayer_Weapon_Kick::Free()
{
	__super::Free();
}
