#include "UI_Player_HP.h"

CUI_Player_HP::CUI_Player_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{
}

CUI_Player_HP::CUI_Player_HP(const CUI& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Player_HP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Player_HP::Initialize(void* pArg)
{
	return S_OK;
}

void CUI_Player_HP::Tick(_float fTimeDelta)
{
}

void CUI_Player_HP::LateTick(_float fTimeDelta)
{
}

HRESULT CUI_Player_HP::Render()
{
	return S_OK;
}

HRESULT CUI_Player_HP::Ready_UI()
{
	// Create

	return S_OK;
}

CUI_Player_HP* CUI_Player_HP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Player_HP* pInstance = new CUI_Player_HP(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Player_HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Player_HP::Clone(void* pArg)
{
	CUI_Player_HP* pInstance = new CUI_Player_HP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Player_HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Player_HP::Pool()
{
	return new CUI_Player_HP(*this);
}

void CUI_Player_HP::Free()
{
}


