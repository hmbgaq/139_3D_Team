#include "UI_Player_Skill.h"
#include "GameInstance.h"

CUI_Player_Skill::CUI_Player_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{
}

CUI_Player_Skill::CUI_Player_Skill(const CUI& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Player_Skill::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Player_Skill::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	return S_OK;
}

void CUI_Player_Skill::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Player_Skill::LateTick(_float fTimeDelta)
{
}

HRESULT CUI_Player_Skill::Render()
{
	return S_OK;
}

HRESULT CUI_Player_Skill::Ready_UI()
{
	// Create

	return S_OK;
}

CUI_Player_Skill* CUI_Player_Skill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Player_Skill* pInstance = new CUI_Player_Skill(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Player_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Player_Skill::Pool()
{
	return new CUI_Player_Skill(*this);
}

CGameObject* CUI_Player_Skill::Clone(void* pArg)
{
	CUI_Player_Skill* pInstance = new CUI_Player_Skill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Player_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Player_Skill::Free()
{
}
