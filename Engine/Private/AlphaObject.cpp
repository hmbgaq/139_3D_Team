#include "..\Public\AlphaObject.h"
#include "GameInstance.h"

CAlphaObject::CAlphaObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CAlphaObject::CAlphaObject(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAlphaObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAlphaObject::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CAlphaObject::Priority_Tick(_float fTimeDelta)
{
}

void CAlphaObject::Tick(_float fTimeDelta)
{
}

void CAlphaObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CAlphaObject::Render()
{
	return S_OK;
}


void CAlphaObject::Compute_CamDistance()
{
	_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	m_fCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

void CAlphaObject::Free()
{
	__super::Free();
}
