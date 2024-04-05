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

	if (m_Time + m_fFrameChangeTime < GetTickCount64())
	{
		++m_iCurrentFrame;

		if (m_iCurrentFrame >= m_iMaxFrame && m_bFinish == false)
		{// Loop

			if(m_bLoop == true)
				m_iCurrentFrame = 0;

			m_bFinish = true;
		}

		m_Time = (DWORD)GetTickCount64();
	}
}

void CUI_Skill_Preview::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
		return;
}

HRESULT CUI_Skill_Preview::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//! 이 셰이더에 0번째 패스로 그린다.
	m_pShaderCom->Begin(m_iShaderNum); //! Shader_PosTex 7번 패스 = VS_MAIN,  PS_UI_HP

	//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
	m_pVIBufferCom->Bind_VIBuffers();

	//! 바인딩된 정점, 인덱스를 그려
	m_pVIBufferCom->Render();

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

	m_bFinish = false;			 // Start -> Go
	m_bSkip = false;			 // Skip

	m_iShaderNum = 0;			 // Defualt

	m_iMaxFrame = 0; // Max

	// 20으로 여태 속도 잘 맞춰서 쓰고있었는데, 왜 인지 모르겠지만 갑자기 속도가 늦음 (갑자기 20으로 잘됨)
	m_fFrameChangeTime = 20.f;	 // FrameSpeed
	m_iCurrentFrame = 0;		 // Frame
}

void CUI_Skill_Preview::Change_Priview(const string& strUIName)
{
	if (m_strPreName != strUIName)
	{
#pragma region 1
		if (strUIName == "Kick")
		{
			m_bLoop = true;
			m_bSkip = false;
			m_bFinish = false;
			m_iMaxFrame = 0;
			m_iCurrentFrame = 0;		 // Frame
		}
		else if (strUIName == "ElectricDash")
		{
			m_bLoop = true;
			m_bSkip = false;
			m_bFinish = false;
			m_iMaxFrame = 0;
			m_iCurrentFrame = 0;		 // Frame
		}
		else if (strUIName == "DashShock")
		{
			m_bLoop = true;
			m_bSkip = false;
			m_bFinish = false;
			m_iMaxFrame = 0;
			m_iCurrentFrame = 0;		 // Frame
		}
		else if (strUIName == "ElectricCord")
		{
			m_bLoop = true;
			m_bSkip = false;
			m_bFinish = false;
			m_iMaxFrame = 0;
			m_iCurrentFrame = 0;		 // Frame
		}
		else if (strUIName == "PowerUP")
		{
			m_bLoop = true;
			m_bSkip = false;
			m_bFinish = false;
			m_iMaxFrame = 0;
			m_iCurrentFrame = 0;		 // Frame
		}
#pragma region 2
		else if (strUIName == "UpperCut")
		{
			m_bLoop = true;
			m_bSkip = false;
			m_bFinish = false;
			m_iMaxFrame = 0;
			m_iCurrentFrame = 0;		 // Frame
		}
		else if (strUIName == "OneTouch")
		{
			m_bLoop = true;
			m_bSkip = false;
			m_bFinish = false;
			m_iMaxFrame = 0;
			m_iCurrentFrame = 0;		 // Frame
		}
		else if (strUIName == "TwoTouch")
		{
			m_bLoop = true;
			m_bSkip = false;
			m_bFinish = false;
			m_iMaxFrame = 0;
			m_iCurrentFrame = 0;		 // Frame
		}
		else if (strUIName == "ThreeTouch")
		{
			m_bLoop = true;
			m_bSkip = false;
			m_bFinish = false;
			m_iMaxFrame = 0;
			m_iCurrentFrame = 0;		 // Frame
		}
		else if (strUIName == "ComboPunch")
		{
			m_bLoop = true;
			m_bSkip = false;
			m_bFinish = false;
			m_iMaxFrame = 0;
			m_iCurrentFrame = 0;		 // Frame
		}
#pragma region 3
		else if (strUIName == "Punch")
		{
			m_bLoop = true;
			m_bSkip = false;
			m_bFinish = false;
			m_iMaxFrame = 0;
			m_iCurrentFrame = 0;		 // Frame
		}
		else if (strUIName == "SuperChargeMod")
		{

		}
		else if (strUIName == "TeleportPunch")
		{

		}
		else if (strUIName == "IncreaseEXP")
		{

		}
		else if (strUIName == "NPCPowerUP")
		{

		}
#pragma region 4
		else if (strUIName == "Heal")
		{

		}
		else if (strUIName == "RecoveryEnergy")
		{

		}
		else if (strUIName == "IncreaseHP")
		{

		}
		else if (strUIName == "IncreaseEnergy")
		{

		}
		else if (strUIName == "MaxHP")
		{

		}
	}

	m_strPreName = strUIName;
}

HRESULT CUI_Skill_Preview::Ready_Components()
{
	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Inventory_Background"),
		TEXT("Com_Texture_PreView"), reinterpret_cast<CComponent**>(&m_pTextureCom[SKILLPREVIEW]))))
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

	if (FAILED(m_pTextureCom[SKILLPREVIEW]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture"/*, m_iCurrentFrame*/)))	// MainStart
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
