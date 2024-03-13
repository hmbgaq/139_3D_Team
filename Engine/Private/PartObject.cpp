#include "..\Public\PartObject.h"

CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CPartObject::CPartObject(const CPartObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pParentTransform = (reinterpret_cast<PARTOBJECT_DESC*>(pArg))->m_pParentTransform;
	if (nullptr == m_pParentTransform)
		return E_FAIL;
	Safe_AddRef(m_pParentTransform);

	return S_OK;
}

void CPartObject::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPartObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPartObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

_float3 CPartObject::Get_WorldPosition()
{
	_float3 result = { 0.f, 0.f, 0.f };
	result.x = m_WorldMatrix._41;
	result.y = m_WorldMatrix._42;
	result.z = m_WorldMatrix._43;

	return result;
}

void CPartObject::Free()
{
	__super::Free();
}
