#include "stdafx.h"
#include "UI_Skill_Preview.h"
#include "GameInstance.h"
#include "Json_Utility.h"

#include "Data_Manager.h"
#include "UI_Manager.h"

CUI_Skill_Preview::CUI_Skill_Preview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_Skill_Preview::CUI_Skill_Preview(const CUI_Skill_Preview& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Skill_Preview::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Skill_Preview::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	Start_Setting();

	return S_OK;
}

void CUI_Skill_Preview::Priority_Tick(_float fTimeDelta)
{

}

void CUI_Skill_Preview::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bActive == true)
	{

	}

}

void CUI_Skill_Preview::Late_Tick(_float fTimeDelta)
{
	if (m_bActive == true)
	{
		if (m_eTextureKind != TEXTURE_END)
		{
			if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
				return;
		}
	}
}

HRESULT CUI_Skill_Preview::Render()
{
	if (m_bActive == true)
	{
		if (m_eTextureKind != TEXTURE_END)
		{
			if (FAILED(Bind_ShaderResources()))
				return E_FAIL;

			//! 이 셰이더에 0번째 패스로 그린다.
			m_pShaderCom->Begin(m_iShaderNum); //! Shader_PosTex 7번 패스 = VS_MAIN,  PS_UI_HP

			//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
			m_pVIBufferCom->Bind_VIBuffers();

			//! 바인딩된 정점, 인덱스를 그려
			m_pVIBufferCom->Render();
		}
	}

	return S_OK;
}

void CUI_Skill_Preview::UI_Ready(_float fTimeDelta)
{
}

void CUI_Skill_Preview::UI_Enter(_float fTimeDelta)
{
}

void CUI_Skill_Preview::UI_Loop(_float fTimeDelta)
{
}

void CUI_Skill_Preview::UI_Exit(_float fTimeDelta)
{
}

void CUI_Skill_Preview::Start_Setting()
{
	//m_pTransformCom->Set_Scaling(g_iWinSizeX, g_iWinSizeY, 0.1f); // Window Size
	//m_pTransformCom->Set_Position(XMVectorSet(0.0f, 0.0f, 0.5f, 1.0f)); // Center Position

}

// 스킬 미리보기 변경
void CUI_Skill_Preview::Change_Preview(const string& strUIName)
{
	if (m_strPreName != strUIName)
	{
#pragma region 1
		if (strUIName == "Kick")
		{
			m_eTextureKind = KICK;
		}
		else if (strUIName == "ElectricDash")
		{
			m_eTextureKind = ELECTRICDASH;
		}
		else if (strUIName == "DashShock")
		{
			m_eTextureKind = DASHSHOCK;
		}
		else if (strUIName == "ElectricCord")
		{
			m_eTextureKind = ELECTRICCORD;
		}
		else if (strUIName == "PowerUP")
		{
			m_eTextureKind = POWERUP;
		}
#pragma region 2
		else if (strUIName == "UpperCut")
		{
			m_eTextureKind = UPPERCUT;
		}
		else if (strUIName == "OneTouch")
		{
			m_eTextureKind = ONETOUCH;
		}
		else if (strUIName == "TwoTouch")
		{
			m_eTextureKind = TWOTOUCH;
		}
		else if (strUIName == "ThreeTouch")
		{
			m_eTextureKind = THREETOUCH;
		}
		else if (strUIName == "ComboPunch")
		{
			m_eTextureKind = COMBOPUNCH;
		}
#pragma region 3
		else if (strUIName == "Punch")
		{
			m_eTextureKind = PUNCH;
		}
		else if (strUIName == "SuperChargeMod")
		{
			m_eTextureKind = SUPERCHARGEMOD;
		}
		else if (strUIName == "TeleportPunch")
		{
			m_eTextureKind = TELEPORTPUNCH;
		}
		else if (strUIName == "IncreaseEXP")
		{
			m_eTextureKind = INCREASEEXP;
		}
		else if (strUIName == "NPCPowerUP")
		{
			m_eTextureKind = NPCPOWERUP;
		}
#pragma region 4
		else if (strUIName == "Heal")
		{
			m_eTextureKind = HEAL;
		}
		else if (strUIName == "RecoveryEnergy")
		{
			m_eTextureKind = RECOVERYENERGY;
		}
		else if (strUIName == "IncreaseHP")
		{
			m_eTextureKind = INCREASEHP;
		}
		else if (strUIName == "IncreaseEnergy")
		{
			m_eTextureKind = INCREASEENERGY;
		}
		else if (strUIName == "MaxHP")
		{
			m_eTextureKind = MAXHP;
		}
	}

	m_strPreName = strUIName;
}

HRESULT CUI_Skill_Preview::Ready_Components()
{
	////! For.Com_Texture // MainStart (LogoLevel)
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Inventory_Background"),
	//	TEXT("Com_Texture_PreView"), reinterpret_cast<CComponent**>(&m_pTextureCom[SKILLPREVIEW]))))
	//	return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Kick"),
		TEXT("Com_Texture_Kick"), reinterpret_cast<CComponent**>(&m_pTextureCom[KICK]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ComboPunch"),
		TEXT("Com_Texture_ComboPunch"), reinterpret_cast<CComponent**>(&m_pTextureCom[COMBOPUNCH]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("DashShock"),
		TEXT("Com_Texture_DashShock"), reinterpret_cast<CComponent**>(&m_pTextureCom[DASHSHOCK]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ElectricCord"),
		TEXT("Com_Texture_ElectricCord"), reinterpret_cast<CComponent**>(&m_pTextureCom[ELECTRICCORD]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ElectricDash"),
		TEXT("Com_Texture_ElectricDash"), reinterpret_cast<CComponent**>(&m_pTextureCom[ELECTRICDASH]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("IncreaseEnergy"),
		TEXT("Com_Texture_IncreaseEnergy"), reinterpret_cast<CComponent**>(&m_pTextureCom[INCREASEENERGY]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("IncreaseEXP"),
		TEXT("Com_Texture_IncreaseEXP"), reinterpret_cast<CComponent**>(&m_pTextureCom[INCREASEEXP]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("IncreaseHP"),
		TEXT("Com_Texture_IncreaseHP"), reinterpret_cast<CComponent**>(&m_pTextureCom[INCREASEHP]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("MaxHP"),
		TEXT("Com_Texture_MaxHP"), reinterpret_cast<CComponent**>(&m_pTextureCom[MAXHP]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("NPCPowerUP"),
		TEXT("Com_Texture_NPCPowerUP"), reinterpret_cast<CComponent**>(&m_pTextureCom[NPCPOWERUP]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("OneTouch"),
		TEXT("Com_Texture_OneTouch"), reinterpret_cast<CComponent**>(&m_pTextureCom[ONETOUCH]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("PowerUP"),
		TEXT("Com_Texture_PowerUP"), reinterpret_cast<CComponent**>(&m_pTextureCom[POWERUP]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Punch"),
		TEXT("Com_Texture_Punch"), reinterpret_cast<CComponent**>(&m_pTextureCom[PUNCH]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("RecoveryEnergy"),
		TEXT("Com_Texture_RecoveryEnergy"), reinterpret_cast<CComponent**>(&m_pTextureCom[RECOVERYENERGY]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SuperChargeMod"),
		TEXT("Com_Texture_SuperChargeMod"), reinterpret_cast<CComponent**>(&m_pTextureCom[SUPERCHARGEMOD]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("TeleportPunch"),
		TEXT("Com_Texture_TeleportPunch"), reinterpret_cast<CComponent**>(&m_pTextureCom[TELEPORTPUNCH]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ThreeTouch"),
		TEXT("Com_Texture_ThreeTouch"), reinterpret_cast<CComponent**>(&m_pTextureCom[THREETOUCH]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("TwoTouch"),
		TEXT("Com_Texture_TwoTouch"), reinterpret_cast<CComponent**>(&m_pTextureCom[TWOTOUCH]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("UpperCut"),
		TEXT("Com_Texture_UpperCut"), reinterpret_cast<CComponent**>(&m_pTextureCom[UPPERCUT]))))
		return E_FAIL;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Heal"),
		TEXT("Com_Texture_Heal"), reinterpret_cast<CComponent**>(&m_pTextureCom[HEAL]))))
		return E_FAIL;

	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* 효과가 필요한 녀석은 Map텍스쳐도 추가해주기 */
	return S_OK;
}

HRESULT CUI_Skill_Preview::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[m_eTextureKind]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture"/*, m_iCurrentFrame*/)))	// MainStart
		return E_FAIL;

	return S_OK;
}

json CUI_Skill_Preview::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);


	/* 추가정보 저장 */


	return out_json;
}

void CUI_Skill_Preview::Load_Desc()
{

}

CUI_Skill_Preview* CUI_Skill_Preview::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Skill_Preview* pInstance = new CUI_Skill_Preview(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Skill_Preview");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Skill_Preview::Clone(void* pArg)
{
	CUI_Skill_Preview* pInstance = new CUI_Skill_Preview(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Skill_Preview");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Skill_Preview::Pool()
{
	return new CUI_Skill_Preview(*this);
}

void CUI_Skill_Preview::Free()
{
	__super::Free();

	for (auto& pTexture : m_pTextureCom)
	{
		if (pTexture != nullptr)
			Safe_Release(pTexture);
	}

}
