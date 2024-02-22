#include "UI_Blood.h"
#include "GameInstance.h"

CUI_Blood::CUI_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{
}

CUI_Blood::CUI_Blood(const CUI& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Blood::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Blood::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	return S_OK;
}

void CUI_Blood::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Blood::LateTick(_float fTimeDelta)
{
}

HRESULT CUI_Blood::Render()
{
	return S_OK;
}

HRESULT CUI_Blood::Ready_UI()
{
	// Create

	return S_OK;
}

CUI_Blood* CUI_Blood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Blood* pInstance = new CUI_Blood(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Blood");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Blood::Pool()
{
	return new CUI_Blood(*this);
}

CGameObject* CUI_Blood::Clone(void* pArg)
{
	CUI_Blood* pInstance = new CUI_Blood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Blood");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Blood::Free()
{
}
