#include "stdafx.h"
#include "..\Public\LandObject.h"

#include "GameInstance.h"

CLandObject::CLandObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CLandObject::CLandObject(const CLandObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void * pArg)
{
	LANDOBJECT_DESC*		pLandDesc = (LANDOBJECT_DESC*)pArg;

	m_pVIBuffer_Terrain = pLandDesc->pTerrainBuffer;
	m_pTransform_Terrain = pLandDesc->pTerrainTransform;

	Safe_AddRef(m_pVIBuffer_Terrain);
	Safe_AddRef(m_pTransform_Terrain);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CLandObject::Priority_Tick(_float fTimeDelta)
{
}

void CLandObject::Tick(_float fTimeDelta)
{
}

void CLandObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}

HRESULT CLandObject::SetUp_OnTerrain(CTransform * pTargetTransform)
{
	_vector		vTargetWorldPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);

	_vector		vLocalPos = XMVector3TransformCoord(vTargetWorldPos, m_pTransform_Terrain->Get_WorldMatrixInverse());

	_float		fHeight = m_pVIBuffer_Terrain->Compute_Height(vLocalPos);

	pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(XMVectorSetY(vLocalPos, fHeight), m_pTransform_Terrain->Get_WorldMatrix()));

	return S_OK;
}

void CLandObject::Free()
{
	__super::Free();


	Safe_Release(m_pVIBuffer_Terrain);
	Safe_Release(m_pTransform_Terrain);
}
