#include "stdafx.h"
#include "UI_AimCrosshair.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "Data_Manager.h"
#include "UI_Manager.h"

CUI_AimCrosshair::CUI_AimCrosshair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_AimCrosshair::CUI_AimCrosshair(const CUI_AimCrosshair& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_AimCrosshair::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_AimCrosshair::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	m_tUIInfo.fScaleX = 5.0f;
	m_tUIInfo.fScaleY = 5.0f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	m_ptOffset = { 0, 0 };

	m_eType = UITYPE::CROSSHAIR;
	m_bActive = true;

	return S_OK;
}

void CUI_AimCrosshair::Priority_Tick(_float fTimeDelta)
{

}

void CUI_AimCrosshair::Tick(_float fTimeDelta)
{
	/* 크로스헤어 UI 이거 그대로 가져다 쓰면 됨 */
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
	{
		//m_pUI_Manager->Active_Crosshair(true);

		//if (m_pGameInstance->Mouse_Down(DIM_LB))
		//	m_pUI_Manager->Trigger_Crosshair(true);
	}
	else
	{
		m_pUI_Manager->Active_Crosshair(false);
		m_pUI_Manager->Trigger_Crosshair(false);
	}

	__super::Tick(fTimeDelta);

	if (m_bActive)
	{

	}
}

void CUI_AimCrosshair::Late_Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return;
	}
}

HRESULT CUI_AimCrosshair::Render()
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

HRESULT CUI_AimCrosshair::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	wstring strPrototag;
	m_pGameInstance->String_To_WString(m_tUIInfo.strProtoTag, strPrototag);

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_AimCrosshair::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;	

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[CROSSHAIR]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;


	return S_OK;
}

json CUI_AimCrosshair::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_AimCrosshair::Load_Desc()
{

}

CUI_AimCrosshair* CUI_AimCrosshair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_AimCrosshair* pInstance = new CUI_AimCrosshair(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_AimCrosshair");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_AimCrosshair::Pool()
{
	return new CUI_AimCrosshair(*this);
}

CGameObject* CUI_AimCrosshair::Clone(void* pArg)
{
	CUI_AimCrosshair* pInstance = new CUI_AimCrosshair(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_AimCrosshair");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_AimCrosshair::Free()
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
