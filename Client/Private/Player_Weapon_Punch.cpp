#include "stdafx.h"
#include "Player_Weapon_Punch.h"
#include "GameInstance.h"
#include "Character.h"
#include "Player.h"
#include "Data_Manager.h"
#include "MasterCamera.h"

CPlayer_Weapon_Punch::CPlayer_Weapon_Punch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CWeapon_Player(pDevice, pContext, strPrototypeTag)
{

}

CPlayer_Weapon_Punch::CPlayer_Weapon_Punch(const CPlayer_Weapon_Punch& rhs)
	: CWeapon_Player(rhs)
{
}

HRESULT CPlayer_Weapon_Punch::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_Punch::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon_Punch::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPlayer_Weapon_Punch::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPlayer_Weapon_Punch::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Weapon_Punch::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_Punch::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	//if (FAILED(__super::Ready_Components()))
	//	return E_FAIL;

	m_iColliderSize = 1;
	m_pColliders.resize(m_iColliderSize);

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::PLAYER_ATTACK);
	BoundingDesc.fRadius = { 0.4f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon_Punch::OnCollisionEnter(CCollider* other)
{
	CCharacter* pTarget_Character = Get_Target_Character(other);
	if (nullptr != pTarget_Character)
	{
		_vector vTargetPos = pTarget_Character->Get_Position_Vector();
		pTarget_Character->Set_Hitted(m_iDamage, Get_Object_Owner()->Calc_Look_Dir_XZ(vTargetPos) * -1 , m_fForce, 1.f, m_eHitDirection, m_eHitPower, true);
		//pTarget_Character->Set_Hitted(0, Get_Object_Owner()->Calc_Look_Dir(vTargetPos) * -1, 0.5f, 1.f, Direction::Front, Power::Light);

		CData_Manager::GetInstance()->Get_Player()->Create_Effect(Get_WorldPosition());

		CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
		if (pSpringCam)
		{
			pSpringCam->Set_ShakeCamera(true);
		}

		m_pGameInstance->Set_RadialBlurTime(0.2f * ECast(m_eHitPower));
		
		
	}
	Set_Enable_Collisions(false);
}

void CPlayer_Weapon_Punch::OnCollisionStay(CCollider* other)
{
	_int a = 0;		
}

void CPlayer_Weapon_Punch::OnCollisionExit(CCollider* other)
{
	_int a = 0;

}

CPlayer_Weapon_Punch* CPlayer_Weapon_Punch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CPlayer_Weapon_Punch* pInstance = new CPlayer_Weapon_Punch(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Weapon_Punch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_Punch::Clone(void* pArg)
{
	CPlayer_Weapon_Punch* pInstance = new CPlayer_Weapon_Punch(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Weapon_Punch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_Punch::Pool()
{
	return new CPlayer_Weapon_Punch(*this);
}

void CPlayer_Weapon_Punch::Free()
{
	__super::Free();
}
