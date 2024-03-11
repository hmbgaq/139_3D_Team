#include "stdafx.h"
#include "UI_LevelUp_MagicSide.h"
#include "GameInstance.h"
#include "Json_Utility.h"

CUI_LevelUp_MagicSide::CUI_LevelUp_MagicSide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_LevelUp_MagicSide::CUI_LevelUp_MagicSide(const CUI_LevelUp_MagicSide& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_LevelUp_MagicSide::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_LevelUp_MagicSide::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	m_eState = UISTATE::LEVEL_UP;
	m_bActive = false;
	m_fAlpha = 0.f;
	m_fLifeTime = 8000.f;
	m_fTime = (_float)GetTickCount64();

	return S_OK;
}

void CUI_LevelUp_MagicSide::Priority_Tick(_float fTimeDelta)
{

}

void CUI_LevelUp_MagicSide::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_LevelUp_MagicSide::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();

	if (m_bReset)
	{
		m_fAlpha = 0.f;
		m_fTime = (_float)GetTickCount64();
	}

	if (m_bActive)
	{
		if (m_pTransformCom->Get_Position().x < 219.f)
		{
			m_pTransformCom->Go_Right(fTimeDelta);
		}
	}

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_LevelUp_MagicSide::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//! 이 셰이더에 0번째 패스로 그릴거야.
	m_pShaderCom->Begin(0); //! Shader_PosTex 7번 패스 = VS_MAIN,  PS_UI_HP

	//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
	m_pVIBufferCom->Bind_VIBuffers();

	//! 바인딩된 정점, 인덱스를 그려
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_LevelUp_MagicSide::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//! For.Com_Texture_Left
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("level_side_element"),
		TEXT("Com_Texture_LEFT_MAGICSIDE"), reinterpret_cast<CComponent**>(&m_pTextureCom[LEFT_MAGICSIDE]))))
		return E_FAIL;

	//! For.Com_Texture_Right (반전시켜야함)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("level_side_element"),
		TEXT("Com_Texture_RIGHT_MAGICSIDE"), reinterpret_cast<CComponent**>(&m_pTextureCom[RIGHT_MAGICSIDE]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LevelUp_MagicSide::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	for (_int i = (_int)0; i < (_int)TEXTURE_END; ++i)
	{
		switch (i)
		{
		case CUI_LevelUp_MagicSide::LEFT_MAGICSIDE:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		}
		case CUI_LevelUp_MagicSide::RIGHT_MAGICSIDE:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture_Second")))
				return E_FAIL;

			break;
		}
		default:
			break;
		}
	}

	return S_OK;
}

json CUI_LevelUp_MagicSide::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_LevelUp_MagicSide::Load_Desc()
{

}

CUI_LevelUp_MagicSide* CUI_LevelUp_MagicSide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_LevelUp_MagicSide* pInstance = new CUI_LevelUp_MagicSide(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_LevelUp_MagicSide");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_LevelUp_MagicSide::Pool()
{
	return new CUI_LevelUp_MagicSide(*this);
}

CGameObject* CUI_LevelUp_MagicSide::Clone(void* pArg)
{
	CUI_LevelUp_MagicSide* pInstance = new CUI_LevelUp_MagicSide(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_LevelUp_MagicSide");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_LevelUp_MagicSide::Free()
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
