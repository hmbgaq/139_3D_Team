#include "UI_Player_HP.h"
#include "GameInstance.h"

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
	if (FAILED(__super::Initialize(pArg))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;


	return S_OK;
}

void CUI_Player_HP::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Player_HP::LateTick(_float fTimeDelta)
{
}

HRESULT CUI_Player_HP::Render()
{
	return S_OK;
}

json CUI_Player_HP::Save_Desc(json& out_json)
{
	json Out_;

	__super::Save_Desc(Out_);

	return Out_;
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
	__super::Free();
}


