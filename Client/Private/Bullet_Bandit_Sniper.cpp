#include "stdafx.h"
#include "Data_Manager.h"
#include "GameInstance.h"
#include "Bullet_Bandit_Sniper.h"
#include "Effect_Manager.h"
#include "Effect_Trail.h"
#include "Data_Manager.h"
#include "Body_Player.h"
#include "Character.h"

CBullet_Bandit_Sniper::CBullet_Bandit_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CProjectile(pDevice, pContext, strPrototypeTag)
{
	/* 이게 있어야 pool에 등록한것을 사용할 수 있음. */
	m_bIsPoolObject = true;
}

CBullet_Bandit_Sniper::CBullet_Bandit_Sniper(const CBullet_Bandit_Sniper& rhs)
	: CProjectile(rhs)
{
}

HRESULT CBullet_Bandit_Sniper::Initialize_Prototype()
{
	return S_OK;
}
	
HRESULT CBullet_Bandit_Sniper::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 60.f;
	GameObjectDesc.fRotationPerSec = 0.f;
	FAILED_CHECK(__super::Initialize(&GameObjectDesc)); /* 컴포넌트 호출 */	

	m_fDamage = 0.f;
	m_fLifeTime = 1.5f;	

	return S_OK;
}

HRESULT CBullet_Bandit_Sniper::Ready_Components()
{
	_int iCurrentLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	{
		BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER_ATTACK);
		BoundingDesc.fRadius = 0.3f;
		BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);

		FAILED_CHECK(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingDesc));
	}


	//! 유정: 트레일 추가
	m_pTrail = EFFECT_MANAGER->Ready_Trail("Monster_Bullet_Trail.json", this);
	m_pTrail->Set_Play(true); // 시작 때 트레일 켜기

	return S_OK;
}

void CBullet_Bandit_Sniper::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBullet_Bandit_Sniper::Tick(_float fTimeDelta)
{

	//! 유정: 트레일
	if (nullptr != m_pTrail)
	{
		//m_pTrail->Set_Play(true); // 트레일 켜기
		m_pTrail->Tick_Trail(fTimeDelta, m_pTransformCom->Get_WorldFloat4x4());
	}

	__super::Tick(fTimeDelta);	
	/* Super에서 m_fLifeTime 을 fTimeDelta 로 감소시키면서 0 이하로 내려가면 알아서 풀에 반납하도록함 + Collider Update */
	/* -> 이동 코드 자체는 구현해야함 */
	//m_pCollider->Update(m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Go_Straight(fTimeDelta);

}

void CBullet_Bandit_Sniper::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


}

HRESULT CBullet_Bandit_Sniper::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CBullet_Bandit_Sniper::Set_Enable(_bool _Enable)
{
	__super::Set_Enable(_Enable);

	//if (nullptr != m_pTrail)
		//m_pTrail->Set_Play(false);

}

void CBullet_Bandit_Sniper::OnCollisionEnter(CCollider* other)
{
	CCharacter* pTarget_Character = Get_Target_Character(other);

	if(pTarget_Character != nullptr)
	{
		m_eHitDirection = Direction::Front;
		m_eHitPower = Power::Medium;
		m_fForce = 0.5f;
	
		//_uint iDamage, _vector vDir, _float fForce, _float fStiffnessRate, Direction eHitDirection, Power eHitPower, _bool bIsMelee)
		pTarget_Character->Set_Hitted(4.f, m_pTransformCom->Get_Look(), m_fForce, 1.f, m_eHitDirection, m_eHitPower);
	}

	if (nullptr != m_pTrail)		// 트레일 끄기
	{
		m_pTrail->Get_Desc()->bRender = false;
		m_pTrail->Set_Play(false);
		m_pTrail->Set_Object_Owner(nullptr);
	}
	Set_Dead(true);
}

void CBullet_Bandit_Sniper::OnCollisionStay(CCollider* other)
{
}

void CBullet_Bandit_Sniper::OnCollisionExit(CCollider* other)
{
}


HRESULT CBullet_Bandit_Sniper::Bind_ShaderResources()
{
	return S_OK;
}

#pragma region Create, Clone, Pool, Free

CBullet_Bandit_Sniper* CBullet_Bandit_Sniper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBullet_Bandit_Sniper* pInstance = new CBullet_Bandit_Sniper(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Bullet_Bandit_Sniper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_Bandit_Sniper::Clone(void* pArg)
{
	CBullet_Bandit_Sniper* pInstance = new CBullet_Bandit_Sniper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Bullet_Bandit_Sniper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_Bandit_Sniper::Pool()
{
	return new CBullet_Bandit_Sniper(*this);
}

void CBullet_Bandit_Sniper::Free()
{
	__super::Free();

	if (nullptr != m_pTrail)		// 트레일 삭제
	{
		m_pTrail->Get_Desc()->bRender = false;
		m_pTrail->Set_Play(false);
		m_pTrail->Set_Object_Owner(nullptr);
		//m_pTrail = nullptr;
		//Safe_Release(m_pTrail);
	}

	//Safe_Release(m_pCollider);
}

#pragma endregion
