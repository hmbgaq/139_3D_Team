#include "stdafx.h"
#include "Body_Monster.h"
#include "GameInstance.h"

CBody_Monster::CBody_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody(pDevice, pContext, strPrototypeTag)
{
}

CBody_Monster::CBody_Monster(const CBody_Monster& rhs)
	: CBody(rhs)
{
}

HRESULT CBody_Monster::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBody_Monster::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CBody_Monster::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Monster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_Monster::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Monster::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CBody_Monster::Render_Shadow()
{
	//FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

void CBody_Monster::OnCollisionEnter(CCollider* other)
{
	if (other->Get_Layer() != ECast(COLLISION_LAYER::MONSTER))
	{
		m_bIsNotUseMovePos = true;
		m_vMovePos = _float3(0.f, 0.f, 0.f);
	}

	__super::OnCollisionEnter(other);
}

void CBody_Monster::OnCollisionStay(CCollider* other)
{
	if (other->Get_Layer() != ECast(COLLISION_LAYER::MONSTER))
	{
		m_bIsNotUseMovePos = true;
		m_vMovePos = _float3(0.f, 0.f, 0.f);
	}

	__super::OnCollisionStay(other);
}

void CBody_Monster::OnCollisionExit(CCollider* other)
{
	m_bIsNotUseMovePos = false;
	__super::OnCollisionExit(other);
}

void CBody_Monster::Free()
{
	__super::Free();
}
