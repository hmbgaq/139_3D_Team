#include "stdafx.h"
#include "UI_Option_Title_Button.h"
#include "GameInstance.h"
#include "Json_Utility.h"

CUI_Option_Title_Button::CUI_Option_Title_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_Option_Title_Button::CUI_Option_Title_Button(const CUI_Option_Title_Button& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Option_Title_Button::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Option_Title_Button::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	return S_OK;
}

void CUI_Option_Title_Button::Priority_Tick(_float fTimeDelta)
{

}

void CUI_Option_Title_Button::Tick(_float fTimeDelta)
{

}

void CUI_Option_Title_Button::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();

	__super::Tick(fTimeDelta);

	if (m_bActive)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_Option_Title_Button::Render()
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

void CUI_Option_Title_Button::UI_Ready(_float fTimeDelta)
{
}

void CUI_Option_Title_Button::UI_Enter(_float fTimeDelta)
{
}

void CUI_Option_Title_Button::UI_Loop(_float fTimeDelta)
{
}

void CUI_Option_Title_Button::UI_Exit(_float fTimeDelta)
{
}

HRESULT CUI_Option_Title_Button::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

#pragma region 아이콘은 텍스처를 골라서 넣을 수 있게 해줘야한다.
	/* 첫 Create는 아무거나 기본 아이콘으로 Initialize에서 테그를 지정해주고, 선택된 텍스처를 파싱하면 파싱된 텍스처를 사용하게 해주자. */
	wstring strPrototag;
	m_pGameInstance->String_To_WString(m_tUIInfo.strProtoTag, strPrototag);
	wstring strPrototagCooldown;

	// 부분 문자열 검색
	size_t found = strPrototag.find(L"ui_icons");

	// 발견된 경우
	if (found != std::wstring::npos) // == 이면 없는것 nullptr처럼 사용
	{
		//! For.Com_Texture2 // NonActive
		if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototag,
			TEXT("Com_Texture_NonActive"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE]))))
			return E_FAIL;

		// 마지막 "_" 위치 찾기
		size_t Lastpos = strPrototag.find_last_of('_');

		// Lastpos "NA"를 "A"로 변경
		strPrototag.replace(Lastpos + 1, 2, L"A");

		//! For.Com_Texture2 // Active
		if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototag,
			TEXT("Com_Texture_Active"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE]))))
			return E_FAIL;
	}
	else //발견되지 않은 경우
	{
		//! For.Com_Texture2 // NonActive
		if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototag,
			TEXT("Com_Texture_NonActive"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE]))))
			return E_FAIL;

		//! For.Com_Texture2 // Active
		if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototag,
			TEXT("Com_Texture_Active"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE]))))
			return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CUI_Option_Title_Button::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (m_bPick == true)
	{
		if (FAILED(m_pTextureCom[ACTIVE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom[NONACTIVE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
	}
	return S_OK;
}

json CUI_Option_Title_Button::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_Option_Title_Button::Load_Desc()
{

}

CUI_Option_Title_Button* CUI_Option_Title_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Option_Title_Button* pInstance = new CUI_Option_Title_Button(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Option_Title_Button");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Option_Title_Button::Pool()
{
	return new CUI_Option_Title_Button(*this);
}

CGameObject* CUI_Option_Title_Button::Clone(void* pArg)
{
	CUI_Option_Title_Button* pInstance = new CUI_Option_Title_Button(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Option_Title_Button");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Option_Title_Button::Free()
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
