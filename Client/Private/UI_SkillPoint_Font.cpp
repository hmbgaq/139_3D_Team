#include "stdafx.h"
#include "UI_SkillPoint_Font.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "Data_Manager.h"

CUI_SkillPoint_Font::CUI_SkillPoint_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CUI_Text(pDevice, pContext, strPrototypeTag)
{

}

CUI_SkillPoint_Font::CUI_SkillPoint_Font(const CUI_SkillPoint_Font& rhs)
	: CUI_Text(rhs)
{
}

HRESULT CUI_SkillPoint_Font::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_SkillPoint_Font::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	m_tUIInfo.fPositionZ = 0.1f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	if (FAILED(Ready_Text())) //! 텍스트 세팅
		return E_FAIL;

	if (FAILED(Find_Change("SkillPoint"))) //! 텍스트 세팅
		return E_FAIL;

	m_fScaleX = m_tUIInfo.fScaleX;
	m_fScaleY = m_tUIInfo.fScaleY;

	m_eType = UITYPE::LEVEL_UP;
	m_bActive = false;

	//m_strText = m_pData_Manager->Get_CurLevel();

	//m_fChangeScale = 4.f;
	m_fAlpha = 0.f;
	//m_fLifeTime = 8000.f;
	//m_fTime = (_float)GetTickCount64();

	return S_OK;
}

void CUI_SkillPoint_Font::Priority_Tick(_float fTimeDelta)
{

}

void CUI_SkillPoint_Font::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Disappear(fTimeDelta);

	if (m_bActive == true)
	{

	}
}

void CUI_SkillPoint_Font::Late_Tick(_float fTimeDelta)
{
	if (m_bActive == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_SkillPoint_Font::Render()
{
	if (m_bActive == true)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		//! 이 셰이더에 0번째 패스로 그릴거야.
		m_pShaderCom->Begin(0); //! Shader_PosTex 7번 패스 = VS_MAIN,  PS_UI_HP

		//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
		m_pVIBufferCom->Bind_VIBuffers();

		//! 바인딩된 정점, 인덱스를 그려
		m_pVIBufferCom->Render();

		if (m_bTrigger == true)
		{
			m_strText = to_wstring(m_pData_Manager->Get_Money());
			//RenderTextWithLineBreak(m_pGameInstance->Convert_WString_To_String(m_strText), 10);
			m_pGameInstance->Render_Font(m_strFontTag, m_strText, _float2((m_pTransformCom->Get_Position().x + (_float)g_iWinSizeX * 0.5f) - 10.f, (-m_pTransformCom->Get_Position().y + (_float)g_iWinSizeY * 0.5f) - 30.f), m_vColor, m_fScale, m_vOrigin, m_fRotation);
		}
	}

	return S_OK;
}

void CUI_SkillPoint_Font::UI_Ready(_float fTimeDelta)
{
}

void CUI_SkillPoint_Font::UI_Enter(_float fTimeDelta)
{
}

void CUI_SkillPoint_Font::UI_Loop(_float fTimeDelta)
{
}

void CUI_SkillPoint_Font::UI_Exit(_float fTimeDelta)
{
}

HRESULT CUI_SkillPoint_Font::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("level_shield"),
		TEXT("Com_Texture_LevShield"), reinterpret_cast<CComponent**>(&m_pTextureCom[SHIELD_FRAME]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillPoint_Font::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[SHIELD_FRAME]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

json CUI_SkillPoint_Font::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_SkillPoint_Font::Load_Desc()
{

}

void CUI_SkillPoint_Font::Add_Text(string strTextKey, string strFontTag, string strText, _float fPosX, _float fPosY, _vector vColor, _float fScale, _float2 vOrigin, _float fRotation)
{
	__super::Add_Text(strTextKey, strFontTag, strText, fPosX, fPosY, vColor, fScale, vOrigin, fRotation);
}

HRESULT CUI_SkillPoint_Font::Find_Change(const string& strTextTag)
{
	__super::Find_Change(strTextTag);
	return S_OK;
}

void CUI_SkillPoint_Font::Check_Disappear(_float fTimeDelta)
{
	if (m_bDisappear == true)
	{
		m_bActive = Alpha_Plus(fTimeDelta);
	}
}

HRESULT CUI_SkillPoint_Font::Ready_Text()
{
	/* 파싱 정보 받기 */
	TEXTINFO* LoadInfo = new TEXTINFO;

	/* 임의 값 (추 후 로드해서 받기) */
	LoadInfo->fPosX = 625.5f;
	LoadInfo->fPosY = 176.f;
	LoadInfo->fScale = 0.7f;
	LoadInfo->vOrigin.x = 0.f;
	LoadInfo->vOrigin.y = 0.f;
	LoadInfo->fRotation = 0.f;
	LoadInfo->strTextKey = "SkillPoint";
	LoadInfo->strFontTag = "Font_EvilWest";
	LoadInfo->strText = to_string(1);
	LoadInfo->vColor.m128_f32[0] = 1.f;
	LoadInfo->vColor.m128_f32[1] = 1.f;
	LoadInfo->vColor.m128_f32[2] = 1.f;
	LoadInfo->vColor.m128_f32[3] = 1.f;

	m_mapText.emplace(LoadInfo->strTextKey, LoadInfo);

	TEXTINFO* pTextInfo = nullptr;

	return S_OK;
}

CUI_SkillPoint_Font* CUI_SkillPoint_Font::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_SkillPoint_Font* pInstance = new CUI_SkillPoint_Font(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_SkillPoint_Font");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_SkillPoint_Font::Pool()
{
	return new CUI_SkillPoint_Font(*this);
}

CGameObject* CUI_SkillPoint_Font::Clone(void* pArg)
{
	CUI_SkillPoint_Font* pInstance = new CUI_SkillPoint_Font(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_SkillPoint_Font");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_SkillPoint_Font::Free()
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
