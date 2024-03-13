#include "stdafx.h"
#include "Data_Manager.h"
#include "GameInstance.h"
#include "Bullet_Bandit_Sniper.h"

CBullet_Bandit_Sniper::CBullet_Bandit_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
	/* 이게 있어야 pool에 등록한것을 사용할 수 있음. */
	m_bIsPoolObject = true;
}

CBullet_Bandit_Sniper::CBullet_Bandit_Sniper(const CBullet_Bandit_Sniper& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBullet_Bandit_Sniper::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet_Bandit_Sniper::Initialize(void* pArg)
{
	BULLET_DESC* desc = reinterpret_cast<BULLET_DESC*>(pArg);

	if (nullptr != desc)
	{
		_float3 pPos = desc->fBullet_InitPos;
		vMoveDir = desc->vBullet_MoveDir;
	}
	FAILED_CHECK(Ready_Components());
	pPos.y += 3.f;
	m_pTransformCom->Set_Pos(pPos);

	return S_OK;
}

void CBullet_Bandit_Sniper::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBullet_Bandit_Sniper::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pTransformCom->Move_Position(vMoveDir, 1.f, fTimeDelta);
}

void CBullet_Bandit_Sniper::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_DebugRender(m_pCollider);
}

HRESULT CBullet_Bandit_Sniper::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CBullet_Bandit_Sniper::Ready_Components()
{
	_int iCurrentLevel = m_pGameInstance->Get_NextLevel();

	/* For. Transform */
	{
		CGameObject::GAMEOBJECT_DESC tTransformDESC = {};
		tTransformDESC.fRotationPerSec = 0.f;
		tTransformDESC.fSpeedPerSec = 20.f;
		FAILED_CHECK(__super::Initialize(&tTransformDESC));

		m_pTransformCom->Set_Pos(250.5f, 5.f, 5.f);
	}

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	{
		BoundingDesc.fRadius = 1.f;
		BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);
		FAILED_CHECK(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingDesc));
		
	}
	
	return S_OK;
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

	Safe_Release(m_pCollider);
}

#pragma endregion
