#include "stdafx.h"
#include "UI_ElementList.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "Renderer.h"

CUI_ElementList::CUI_ElementList(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_ElementList::CUI_ElementList(const CUI_ElementList& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_ElementList::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_ElementList::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	m_bButtonUI = true;

	m_fWithProgress = -1.f;

	return S_OK;
}

void CUI_ElementList::Priority_Tick(_float fTimeDelta)
{

}

void CUI_ElementList::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Picking(fTimeDelta);

	if (m_bActive == true)
	{
		if (m_fWithProgress < 1.f)
			m_fWithProgress += fTimeDelta;
		else
			m_fWithProgress = -1.f;
	}
}

void CUI_ElementList::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();


	if (m_bActive == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_ElementList::Render()
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

void CUI_ElementList::UI_Ready(_float fTimeDelta)
{
}

void CUI_ElementList::UI_Enter(_float fTimeDelta)
{
}

void CUI_ElementList::UI_Loop(_float fTimeDelta)
{
}

void CUI_ElementList::UI_Exit(_float fTimeDelta)
{
}

void CUI_ElementList::Check_Picking(_float fTimeDelta)
{
	if (m_bPick == false)
		return;

	if (g_UIMouseDownLB == true)
	{
		if (m_tUIInfo.strUIName == "HBAO")
		{
			m_bHABO_Active = !m_bHABO_Active;
			m_pGameInstance->Get_Renderer()->Set_HBAO_Active(m_bHABO_Active);
		}
		else if (m_tUIInfo.strUIName == "FOG")
		{
			m_bFOG_Active = !m_bFOG_Active;
			m_pGameInstance->Get_Renderer()->Set_Fog_Active(m_bFOG_Active);
		}
		else if (m_tUIInfo.strUIName == "RADIAL_BLUR")
		{
			m_bRadial_Blur_Active = !m_bRadial_Blur_Active;
			m_pGameInstance->Get_Renderer()->Set_Radial_Blur_Active(m_bRadial_Blur_Active);
		}
		else if (m_tUIInfo.strUIName == "DOF")
		{
			m_bDof_Active = !m_bDof_Active;
			m_pGameInstance->Get_Renderer()->Set_DOF_Active(m_bDof_Active);
		}
		else if (m_tUIInfo.strUIName == "HDR")
		{
			m_bHDR_Active = !m_bHDR_Active;
			m_pGameInstance->Get_Renderer()->Set_HDR_Active(m_bHDR_Active);
		}
		else if (m_tUIInfo.strUIName == "SHADOW")
		{
			m_bShadow_Active = !m_bShadow_Active;
			m_pGameInstance->Get_Renderer()->Set_Shadow_Active(m_bShadow_Active);
		}
		else if (m_tUIInfo.strUIName == "PBR")
		{
			m_bPBR_Active = !m_bPBR_Active;
			m_pGameInstance->Get_Renderer()->Set_PBR_Active(m_bPBR_Active);
		}
	}
}

HRESULT CUI_ElementList::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

#pragma region 
	//! For.Com_Texture2 // NonActive
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_tab_DOF"),
		TEXT("Com_Texture_Element_List_NonActive_DOF"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE_DOF]))))
		return E_FAIL;

	//! For.Com_Texture2 // Active
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_navigation_DOF"),
		TEXT("Com_Texture_Element_List_Active_DOF"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE_DOF]))))
		return E_FAIL;
#pragma endregion

#pragma region 
	//! For.Com_Texture2 // NonActive
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_tab_FOG"),
		TEXT("Com_Texture_Element_List_NonActive_FOG"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE_FOG]))))
		return E_FAIL;

	//! For.Com_Texture2 // Active
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_navigation_FOG"),
		TEXT("Com_Texture_Element_List_Active_FOG"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE_FOG]))))
		return E_FAIL;
#pragma endregion

#pragma region 
	//! For.Com_Texture2 // NonActive
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_tab_HDR"),
		TEXT("Com_Texture_Element_List_NonActive_HDR"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE_HDR]))))
		return E_FAIL;

	//! For.Com_Texture2 // Active
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_navigation_HDR"),
		TEXT("Com_Texture_Element_List_Active_HDR"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE_HDR]))))
		return E_FAIL;
#pragma endregion

#pragma region 
	//! For.Com_Texture2 // NonActive
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_tab_HBAO"),
		TEXT("Com_Texture_Element_List_NonActive_HABO"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE_HABO]))))
		return E_FAIL;

	//! For.Com_Texture2 // Active
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_navigation_HBAO"),
		TEXT("Com_Texture_Element_List_Active_HABO"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE_HABO]))))
		return E_FAIL;
#pragma endregion

#pragma region HSV
	//! For.Com_Texture2 // NonActive
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_tab_HSV"),
		TEXT("Com_Texture_Element_List_NonActive_HSV"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE_HSV]))))
		return E_FAIL;

	//! For.Com_Texture2 // Active
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_navigation_HSV"),
		TEXT("Com_Texture_Element_List_Active_HSV"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE_HSV]))))
		return E_FAIL;
#pragma endregion

#pragma region PBR
	////! For.Com_Texture2 // NonActive
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_tab_PBR"),
	//	TEXT("Com_Texture_Element_List_NonActive"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE]))))
	//	return E_FAIL;

	////! For.Com_Texture2 // Active
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_navigation_PBR"),
	//	TEXT("Com_Texture_Element_List_Active"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE]))))
	//	return E_FAIL;
#pragma endregion

#pragma region RADIAL_BLUR
	//! For.Com_Texture2 // NonActive
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_tab_RADIAL"),
		TEXT("Com_Texture_Element_List_NonActive_BLUR"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE_RADIAL_BLUR]))))
		return E_FAIL;

	//! For.Com_Texture2 // Active
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_navigation_RADIAL"),
		TEXT("Com_Texture_Element_List_Active_BLUR"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE_RADIAL_BLUR]))))
		return E_FAIL;
#pragma endregion

#pragma region PBR
	//! For.Com_Texture2 // NonActive
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_tab_PBR"),
		TEXT("Com_Texture_Element_List_NonActive_PBR"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE_PBR]))))
		return E_FAIL;

	//! For.Com_Texture2 // Active
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_navigation_PBR"),
		TEXT("Com_Texture_Element_List_Active_PBR"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE_PBR]))))
		return E_FAIL;
#pragma endregion

#pragma region SHADOW
	//! For.Com_Texture2 // NonActive
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_tab_SHADOW"),
		TEXT("Com_Texture_Element_List_NonActive_SHADOW"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE_SHADOW]))))
		return E_FAIL;

	//! For.Com_Texture2 // Active
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_navigation_SHADOW"),
		TEXT("Com_Texture_Element_List_Active_SHADOW"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE_SHADOW]))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CUI_ElementList::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_WithProgress", &m_fWithProgress, sizeof(_float))))
		return E_FAIL;

	if (m_bPick == true)
	{
		/* 이 객체가 주가되는 녀석이니, 본인이 가진 변수들로 픽킹을 구분하고, 이 객체가 픽킹됐는지 알아야하는 객체는 매니저를 통해 알게하자. */
		if (m_tUIInfo.strUIName == "HBAO") // 선택된 녀석과 본인이 가진 이름을 비교해서 띄워야한다.
		{
			if (FAILED(m_pTextureCom[ACTIVE_HABO]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "FOG")
		{
			if (FAILED(m_pTextureCom[ACTIVE_FOG]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "RADIAL_BLUR")
		{
			if (FAILED(m_pTextureCom[ACTIVE_RADIAL_BLUR]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "DOF")
		{
			if (FAILED(m_pTextureCom[ACTIVE_DOF]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "HDR")
		{
			if (FAILED(m_pTextureCom[ACTIVE_HDR]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "SHADOW")
		{
			if (FAILED(m_pTextureCom[ACTIVE_SHADOW]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "PBR")
		{
			if (FAILED(m_pTextureCom[ACTIVE_PBR]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "HSV")
		{
			if (FAILED(m_pTextureCom[ACTIVE_HSV]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
	}
	else
	{
		if (m_tUIInfo.strUIName == "HBAO")
		{
			if (FAILED(m_pTextureCom[NONACTIVE_HABO]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "FOG")
		{
			if (FAILED(m_pTextureCom[NONACTIVE_FOG]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "RADIAL_BLUR")
		{
			if (FAILED(m_pTextureCom[NONACTIVE_RADIAL_BLUR]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "DOF")
		{
			if (FAILED(m_pTextureCom[NONACTIVE_DOF]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "HDR")
		{
			if (FAILED(m_pTextureCom[NONACTIVE_HDR]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "SHADOW")
		{
			if (FAILED(m_pTextureCom[NONACTIVE_SHADOW]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "PBR")
		{
			if (FAILED(m_pTextureCom[NONACTIVE_PBR]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else if (m_tUIInfo.strUIName == "HSV")
		{
			if (FAILED(m_pTextureCom[NONACTIVE_HSV]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
	}
	return S_OK;
}

json CUI_ElementList::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_ElementList::Load_Desc()
{

}

CUI_ElementList* CUI_ElementList::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_ElementList* pInstance = new CUI_ElementList(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_ElementList");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_ElementList::Pool()
{
	return new CUI_ElementList(*this);
}

CGameObject* CUI_ElementList::Clone(void* pArg)
{
	CUI_ElementList* pInstance = new CUI_ElementList(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_ElementList");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_ElementList::Free()
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
