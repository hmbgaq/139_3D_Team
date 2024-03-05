#include "stdafx.h"
#include "UI_Player_Skill_Frame.h"
#include "GameInstance.h"
#include "Json_Utility.h"

CUI_Player_Skill_Frame::CUI_Player_Skill_Frame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_Player_Skill_Frame::CUI_Player_Skill_Frame(const CUI_Player_Skill_Frame& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Player_Skill_Frame::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Player_Skill_Frame::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	return S_OK;
}

void CUI_Player_Skill_Frame::Priority_Tick(_float fTimeDelta)
{

}

void CUI_Player_Skill_Frame::Tick(_float fTimeDelta)
{

}

void CUI_Player_Skill_Frame::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();

	__super::Tick(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_Player_Skill_Frame::Render()
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

HRESULT CUI_Player_Skill_Frame::Ready_Components()
{
	//if (FAILED(__super::Ready_Components())); // Ready : Texture / MapTexture
	//	return E_FAIL;

	//! For.Com_Texture1
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_energy_empty"),
		TEXT("Com_Texture_EnergyEmpty"), reinterpret_cast<CComponent**>(&m_pTextureCom[ENERGY_EMPTY]))))
		return E_FAIL;

	//! For.Com_Texture2
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_energy_powering_up"),
		TEXT("Com_Texture_EnergyGray"), reinterpret_cast<CComponent**>(&m_pTextureCom[ENERGY_GRAY]))))
		return E_FAIL;

	//! For.Com_Texture3
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_energy_ready"),
		TEXT("Com_Texture_EnergyYellow"), reinterpret_cast<CComponent**>(&m_pTextureCom[ENERGY_YELLOW]))))
		return E_FAIL;
	
	//! For.Com_Texture4
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_energy_blue"),
		TEXT("Com_Texture_EnergyBlue"), reinterpret_cast<CComponent**>(&m_pTextureCom[ENERGY_BLUE]))))
		return E_FAIL;

	//! For.Com_Shader
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Player_Skill_Frame::Bind_ShaderResources()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));


	for (_int i = (_int)0; i < (_int)TEXTURE_END; ++i)
	{
		switch (i)
		{
		case CUI_Player_Skill_Frame::ENERGY_EMPTY:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		}
		case CUI_Player_Skill_Frame::ENERGY_GRAY:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture_Second")))
				return E_FAIL;

			break;
		}
		case CUI_Player_Skill_Frame::ENERGY_YELLOW:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture_Third"))) // error : 던져줄 전역 그릇이 없어서 FAIL 탄것..
				return E_FAIL;
			break;
		}
		case CUI_Player_Skill_Frame::ENERGY_BLUE:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture_Fourth")))
				return E_FAIL;
			break;
		}
		case CUI_Player_Skill_Frame::TEXTURE_END:
			break;
		default:
			break;
		}
	}

	return S_OK;
}

json CUI_Player_Skill_Frame::Save_Desc(json& out_json)
{
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_Player_Skill_Frame::Load_Desc()
{

}

CUI_Player_Skill_Frame* CUI_Player_Skill_Frame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Player_Skill_Frame* pInstance = new CUI_Player_Skill_Frame(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Player_Skill_Frame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Player_Skill_Frame::Pool()
{
	return new CUI_Player_Skill_Frame(*this);
}

CGameObject* CUI_Player_Skill_Frame::Clone(void* pArg)
{
	CUI_Player_Skill_Frame* pInstance = new CUI_Player_Skill_Frame(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Player_Skill_Frame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Player_Skill_Frame::Free()
{
	__super::Free();

	//Safe_Release(m_pVIBufferCom);
	//Safe_Release(m_pShaderCom);

	for (auto& pTexture : m_pTextureCom)
	{
		if(pTexture != nullptr)
			Safe_Release(pTexture);
			//Safe_Release(pTexture);
	}

}
