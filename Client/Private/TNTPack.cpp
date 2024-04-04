#include "..\Public\TNTPack.h"
#include "Model.h"
#include "Shader.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Character.h"

CTNTPack::CTNTPack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CDestructableProp(pDevice, pContext, strPrototypeTag)
{
}

CTNTPack::CTNTPack(const CTNTPack& rhs)
	: CDestructableProp(rhs)
{
}

HRESULT CTNTPack::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CTNTPack::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CTNTPack::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CTNTPack::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTNTPack::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CTNTPack::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CTNTPack::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

void CTNTPack::Explode()
{
	CGameObject* pBullet = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_MONSTER_BULLET, L"Prototype_GameObject_Explosion_TNTPack");
	_float3 vSpawnPos = Get_Position();
	pBullet->Set_Position(vSpawnPos);
	Set_Dead(true);
}

void CTNTPack::OnCollision(CCollider* other)
{
	if (nullptr == other || false == other->Get_Enable()) return;

	if (other->Get_Layer() == ECast(COLLISION_LAYER::MONSTER)
		|| other->Get_Layer() == ECast(COLLISION_LAYER::PLAYER))
	{
		//CGameObject* pOwner = other->Get_Owner();
		//if (nullptr == pOwner) return;
		//CCharacter* pParentCharacter = dynamic_cast<CCharacter*>(pOwner->Get_Object_Owner());
		//if (pParentCharacter)
		//{
		//	_vector vTargetPos = pParentCharacter->Get_Position_Vector();
		//	pParentCharacter->Add_Force(Calc_Look_Dir_XZ(vTargetPos) * -1, 12.f * m_pGameInstance->Get_TimeDelta());
		//}
	}
	else
	{
		Explode();
	}
}


void CTNTPack::OnCollisionEnter(CCollider* other)
{
	OnCollision(other);
}

void CTNTPack::OnCollisionStay(CCollider* other)
{
	OnCollision(other);
}

void CTNTPack::OnCollisionExit(CCollider* other)
{
}

HRESULT CTNTPack::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrnetLevel, L"Prototype_Component_Model_TNTPack",
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::DESTRUCTABLE_PROPS);
	BoundingDesc.vExtents = _float3(0.3f, 0.3f, 0.3f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

CTNTPack* CTNTPack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CTNTPack* pInstance = new CTNTPack(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTNTPack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTNTPack::Clone(void* pArg)
{
	CTNTPack* pInstance = new CTNTPack(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTNTPack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTNTPack::Pool()
{
	return new CTNTPack(*this);
}

void CTNTPack::Free()
{
	__super::Free();
}
