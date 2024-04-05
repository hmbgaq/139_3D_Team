#include "..\Public\TNTCrate.h"
#include "Model.h"
#include "Shader.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Character.h"

CTNTCrate::CTNTCrate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CDestructableProp(pDevice, pContext, strPrototypeTag)
{
}

CTNTCrate::CTNTCrate(const CTNTCrate& rhs)
	: CDestructableProp(rhs)
{
}

HRESULT CTNTCrate::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CTNTCrate::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CTNTCrate::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CTNTCrate::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTNTCrate::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CTNTCrate::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CTNTCrate::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

void CTNTCrate::Explode()
{
	CGameObject* pBullet = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_MONSTER_BULLET, L"Prototype_GameObject_Explosion_TNTCrate");
	_float3 vSpawnPos = Get_Position();
	pBullet->Set_Position(vSpawnPos);
	Set_Dead(true);
}

void CTNTCrate::OnCollision(CCollider* other)
{
	if (nullptr == other || false == other->Get_Enable()) return;

	if (other->Get_Layer() == ECast(COLLISION_LAYER::MONSTER)
		|| other->Get_Layer() == ECast(COLLISION_LAYER::PLAYER))
	{
		CGameObject* pOwner = other->Get_Owner();
		if (nullptr == pOwner) return;
		CCharacter* pParentCharacter = dynamic_cast<CCharacter*>(pOwner->Get_Object_Owner());
		if (pParentCharacter)
		{
			_vector vTargetPos = pParentCharacter->Get_Position_Vector();
			pParentCharacter->Add_Force(Calc_Look_Dir_XZ(vTargetPos) * -1, 12.f * m_pGameInstance->Get_TimeDelta());
		}
	}
	else 
	{
		Explode();
	}
}


void CTNTCrate::OnCollisionEnter(CCollider* other)
{
	OnCollision(other);
}

void CTNTCrate::OnCollisionStay(CCollider* other)
{
	OnCollision(other);
}

void CTNTCrate::OnCollisionExit(CCollider* other)
{
}

HRESULT CTNTCrate::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrnetLevel, L"Prototype_Component_Model_TNTCrate",
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::DESTRUCTABLE_PROPS);
	BoundingDesc.vExtents = _float3(1.0f, 1.0f, 1.0f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

CTNTCrate* CTNTCrate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CTNTCrate* pInstance = new CTNTCrate(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTNTCrate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTNTCrate::Clone(void* pArg)
{
	CTNTCrate* pInstance = new CTNTCrate(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTNTCrate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTNTCrate::Pool()
{
	return new CTNTCrate(*this);
}

void CTNTCrate::Free()
{
	__super::Free();
}
