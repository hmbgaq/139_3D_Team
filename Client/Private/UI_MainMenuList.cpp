#include "stdafx.h"
#include "UI_MainMenuList.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "Renderer.h"
#include "Data_Manager.h"
#include "UI_Manager.h"

CUI_MainMenuList::CUI_MainMenuList(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_MainMenuList::CUI_MainMenuList(const CUI_MainMenuList& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_MainMenuList::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_MainMenuList::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	m_bButtonUI = true;

	if (m_tUIInfo.strUIName == "GAMEPLAY")
	{
		m_bActive = true;
		m_bPlayAnim = true;
	}
	else if (m_tUIInfo.strUIName == "INTRO")
		m_bActive = false;
	else if (m_tUIInfo.strUIName == "INTROBOSS")
		m_bActive = false;
	else if (m_tUIInfo.strUIName == "SNOWMOUNTAIN")
		m_bActive = false;
	else if (m_tUIInfo.strUIName == "SNOWMOUNTAINBOSS")
		m_bActive = false;
	else if (m_tUIInfo.strUIName == "TOOL")
	{
		m_bActive = true;
		m_bPlayAnim = true;
	}
	else if (m_tUIInfo.strUIName == "EXIT")
	{
		m_bActive = true;
		m_bPlayAnim = true;
	}

	return S_OK;
}

void CUI_MainMenuList::Priority_Tick(_float fTimeDelta)
{

}

void CUI_MainMenuList::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bActive == true)
	{
		Check_Picking(fTimeDelta);
	}
}

void CUI_MainMenuList::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();

	if (m_bActive == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_MainMenuList::Render()
{
	if (m_bActive)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		//! 이 셰이더에 0번째 패스로 그릴거야.
		m_pShaderCom->Begin(0); //! Shader_PosTex 7번 패스 = VS_MAIN,  PS_UI_HP

		//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
		m_pVIBufferCom->Bind_VIBuffers();

		//! 바인딩된 정점, 인덱스를 그려
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

void CUI_MainMenuList::UI_Ready(_float fTimeDelta)
{
}

void CUI_MainMenuList::UI_Enter(_float fTimeDelta)
{
}

void CUI_MainMenuList::UI_Loop(_float fTimeDelta)
{
}

void CUI_MainMenuList::UI_Exit(_float fTimeDelta)
{
}

void CUI_MainMenuList::Check_Picking(_float fTimeDelta)
{
	if (m_bPick == true)
	{
		if (m_bSelect == true || m_bSelectPressing == true)
		{
			if (m_tUIInfo.strUIName == "GAMEPLAY")
			{
				m_pUIManager->Active_LevelList();
				m_pUIManager->NonActive_MainList();
				m_bPick = false;
				m_bSelect = false;
				m_bSelectPressing = false;
			}
			else if (m_tUIInfo.strUIName == "INTRO")
			{
				m_pData_Manager->Set_SelectLevel(LEVEL_INTRO);
			}
			else if (m_tUIInfo.strUIName == "INTROBOSS")
			{
				m_pData_Manager->Set_SelectLevel(LEVEL_INTRO_BOSS);
			}
			else if (m_tUIInfo.strUIName == "SNOWMOUNTAIN")
			{
				m_pData_Manager->Set_SelectLevel(LEVEL_SNOWMOUNTAIN);
			}
			else if (m_tUIInfo.strUIName == "SNOWMOUNTAINBOSS")
			{
				m_pData_Manager->Set_SelectLevel(LEVEL_SNOWMOUNTAINBOSS);
			}
			else if (m_tUIInfo.strUIName == "TOOL")
			{
				m_pData_Manager->Set_SelectLevel(LEVEL_TOOL);
			}
			else if (m_tUIInfo.strUIName == "EXIT")
			{
				//g_CloseWindow = true; // Close Window
			}
		}
	}
}

HRESULT CUI_MainMenuList::Ready_Components()
{
	_uint iLevel = 0;

	if (m_pGameInstance->Get_NextLevel() == (_uint)LEVEL_TOOL)
		iLevel = (_uint)LEVEL_TOOL;
	if (m_pGameInstance->Get_NextLevel() == (_uint)LEVEL_LOGO)
		iLevel = (_uint)LEVEL_LOGO;

	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

#pragma region
	//! For.Com_Texture2 // 
	if (FAILED(__super::Add_Component(iLevel, TEXT("MainMenu_GamePlay"),
		TEXT("Com_Texture_MainMenu_GamePlay"), reinterpret_cast<CComponent**>(&m_pTextureCom[GAMEPLAY]))))
		return E_FAIL;

	//! For.Com_Texture2 // 
	if (FAILED(__super::Add_Component(iLevel, TEXT("MainMenu_Intro"),
		TEXT("Com_Texture_MainMenu_Intro"), reinterpret_cast<CComponent**>(&m_pTextureCom[INTRO]))))
		return E_FAIL;

	//! For.Com_Texture2 // 
	if (FAILED(__super::Add_Component(iLevel, TEXT("MainMenu_IntroBoss"),
		TEXT("Com_Texture_MainMenu_IntroBoss"), reinterpret_cast<CComponent**>(&m_pTextureCom[INTRO_BOSS]))))
		return E_FAIL;

	//! For.Com_Texture2 // 
	if (FAILED(__super::Add_Component(iLevel, TEXT("MainMenu_SnowMountain"),
		TEXT("Com_Texture_MainMenu_SnowMountain"), reinterpret_cast<CComponent**>(&m_pTextureCom[SNOWMOUNTAIN]))))
		return E_FAIL;

	//! For.Com_Texture2 // 
	if (FAILED(__super::Add_Component(iLevel, TEXT("MainMenu_SnowMountainBoss"),
		TEXT("Com_Texture_MainMenu_SnowMountainBoss"), reinterpret_cast<CComponent**>(&m_pTextureCom[SNOWMOUNTAIN_BOSS]))))
		return E_FAIL;
	
	//! For.Com_Texture2 // 
	if (FAILED(__super::Add_Component(iLevel, TEXT("MainMenu_Tool"),
		TEXT("Com_Texture_MainMenu_Tool"), reinterpret_cast<CComponent**>(&m_pTextureCom[TOOL]))))
		return E_FAIL;

	//! For.Com_Texture2 // 
	if (FAILED(__super::Add_Component(iLevel, TEXT("MainMenu_Exit"),
		TEXT("Com_Texture_MainMenu_Exit"), reinterpret_cast<CComponent**>(&m_pTextureCom[EXIT]))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CUI_MainMenuList::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	/* 이 객체가 주가되는 녀석이니, 본인이 가진 변수들로 픽킹을 구분하고, 이 객체가 픽킹됐는지 알아야하는 객체는 매니저를 통해 알게하자. */
	if (m_tUIInfo.strUIName == "GAMEPLAY") // 선택된 녀석과 본인이 가진 이름을 비교해서 띄워야한다.
	{
		if (FAILED(m_pTextureCom[GAMEPLAY]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else if (m_tUIInfo.strUIName == "INTRO")
	{
		if (FAILED(m_pTextureCom[INTRO]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else if (m_tUIInfo.strUIName == "INTROBOSS")
	{
		if (FAILED(m_pTextureCom[INTRO_BOSS]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else if (m_tUIInfo.strUIName == "SNOWMOUNTAIN")
	{
		if (FAILED(m_pTextureCom[SNOWMOUNTAIN]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else if (m_tUIInfo.strUIName == "SNOWMOUNTAINBOSS")
	{
		if (FAILED(m_pTextureCom[SNOWMOUNTAIN_BOSS]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else if (m_tUIInfo.strUIName == "TOOL")
	{
		if (FAILED(m_pTextureCom[TOOL]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else if (m_tUIInfo.strUIName == "EXIT")
	{
		if (FAILED(m_pTextureCom[EXIT]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
	}
	
	return S_OK;
}

json CUI_MainMenuList::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_MainMenuList::Load_Desc()
{

}

CUI_MainMenuList* CUI_MainMenuList::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_MainMenuList* pInstance = new CUI_MainMenuList(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_MainMenuList");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_MainMenuList::Pool()
{
	return new CUI_MainMenuList(*this);
}

CGameObject* CUI_MainMenuList::Clone(void* pArg)
{
	CUI_MainMenuList* pInstance = new CUI_MainMenuList(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_MainMenuList");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MainMenuList::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	for (auto& pTexture : m_pTextureCom)
	{
		if (pTexture != nullptr)
			Safe_Release(pTexture);
	}

}
