#include "stdafx.h"
#include "UI_ItemIcon.h"
#include "GameInstance.h"
#include "Json_Utility.h"

CUI_ItemIcon::CUI_ItemIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_ItemIcon::CUI_ItemIcon(const CUI_ItemIcon& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_ItemIcon::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_ItemIcon::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	m_tUIInfo.fScaleX = 55.0f;
	m_tUIInfo.fScaleY = 55.0f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	m_eType = UITYPE::REWARD;

	return S_OK;
}

void CUI_ItemIcon::Priority_Tick(_float fTimeDelta)
{

}

void CUI_ItemIcon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Check_Disappear(fTimeDelta);

	if (m_bActive == true)
	{
	}
}

void CUI_ItemIcon::Late_Tick(_float fTimeDelta)
{
	if (m_bActive == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_ItemIcon::Render()
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
	}
	return S_OK;
}

void CUI_ItemIcon::UI_Ready(_float fTimeDelta)
{
}

void CUI_ItemIcon::UI_Enter(_float fTimeDelta)
{
}

void CUI_ItemIcon::UI_Loop(_float fTimeDelta)
{
}

void CUI_ItemIcon::UI_Exit(_float fTimeDelta)
{
}

HRESULT CUI_ItemIcon::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//! For.Com_Texture_Sunbeacon
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("perk_element"),
		TEXT("Com_Texture_Sunbeacon"), reinterpret_cast<CComponent**>(&m_pTextureCom[ITEM_SUNBEACON]))))
		return E_FAIL;
	
	//! For.Com_Texture_ItemGold
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_icon_gold_reward"),
		TEXT("Com_Texture_ItemGold"), reinterpret_cast<CComponent**>(&m_pTextureCom[ITEM_GOLD]))))
		return E_FAIL;

	//! For.Com_Texture_UnlockBlock
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_icons_projectile_block"),
		TEXT("Com_Texture_UnlockBlock"), reinterpret_cast<CComponent**>(&m_pTextureCom[UNLOCKED_BLOCK]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ItemIcon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	string TestName = m_tUIInfo.strObjectName;
	for (_int i = (_int)0; i < (_int)TEXTURE_END; ++i)
	{
		switch (i)
		{
		case CUI_ItemIcon::ITEM_SUNBEACON:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		}
		case CUI_ItemIcon::ITEM_GOLD:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture_Second")))
				return E_FAIL;

			break;
		}
		case CUI_ItemIcon::UNLOCKED_BLOCK:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture_Third")))
				return E_FAIL;

			break;
		}
		default:
			break;
		}
	}

	return S_OK;
}

json CUI_ItemIcon::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_ItemIcon::Load_Desc()
{

}

CUI_ItemIcon* CUI_ItemIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_ItemIcon* pInstance = new CUI_ItemIcon(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_ItemIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_ItemIcon::Pool()
{
	return new CUI_ItemIcon(*this);
}

CGameObject* CUI_ItemIcon::Clone(void* pArg)
{
	CUI_ItemIcon* pInstance = new CUI_ItemIcon(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_ItemIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_ItemIcon::Free()
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
