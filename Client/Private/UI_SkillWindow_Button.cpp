#include "stdafx.h"
#include "UI_SkillWindow_Button.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "Renderer.h"
#include "UI_Manager.h"

CUI_SkillWindow_Button::CUI_SkillWindow_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_SkillWindow_Button::CUI_SkillWindow_Button(const CUI_SkillWindow_Button& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_SkillWindow_Button::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_SkillWindow_Button::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	m_bButtonUI = true;

	m_fWithProgress = -1.f;
	m_fChangeScale = 2.f;

	m_fScaleX = 140.f;
	m_fScaleY = 70.f;

	if (m_tUIInfo.strProtoTag == "SkillButton")
		m_bSelectButton = true;
	else
	{
		m_bSelectButton = false;
	}

	return S_OK;
}

void CUI_SkillWindow_Button::Priority_Tick(_float fTimeDelta)
{

}

void CUI_SkillWindow_Button::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bActive == true)
	{
		if (m_fWithProgress < 1.f)
			m_fWithProgress += fTimeDelta;
		else
			m_fWithProgress = -1.f;

		Check_Picking(fTimeDelta);
		Check_State(fTimeDelta);
		
	}
}

void CUI_SkillWindow_Button::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();


	if (m_bActive == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_SkillWindow_Button::Render()
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

void CUI_SkillWindow_Button::UI_Ready(_float fTimeDelta)
{
}

void CUI_SkillWindow_Button::UI_Enter(_float fTimeDelta)
{
}

void CUI_SkillWindow_Button::UI_Loop(_float fTimeDelta)
{
}

void CUI_SkillWindow_Button::UI_Exit(_float fTimeDelta)
{
}

void CUI_SkillWindow_Button::Check_Picking(_float fTimeDelta)
{
	if (m_bPick == true)
	{
		if (m_bSoundOk == false)
		{
			wstring strFileName = L"";
			strFileName = L"menu_options_highlight.wav";

			m_pGameInstance->Play_Sound(L"UI_MouseOver", strFileName, CHANNELID::SOUND_UI_MOUSEOVER, 10.f);

			m_bSoundOk = true;
		}

		if (g_UIMouseDownLB == true)
		{
			if (m_bSoundOk2 == false)
			{
				wstring strFileName = L"";
				strFileName = L"HM_UI_Unlock_Upgrade_1s_01.wav";

				m_pGameInstance->Play_Sound(L"UI_SkillWindow", strFileName, CHANNELID::SOUND_UI_MOUSECLICK, 10.f);

				m_bSoundOk2 = true;
			}

			if (m_tUIInfo.strProtoTag == "WeaponButton" ||
				m_tUIInfo.strProtoTag == "WeaponButtonActive")
			{
				m_bSelectButton = true;
				m_pUIManager->Select_SkillWindowButton("WeaponButton", false); // 선택한 버튼을 제외하고 모두 꺼준다.
				//m_pUIManager->ActiveWeapon();O
				//m_pUIManager->NonActiveSkill();
				m_pUIManager->Active_WeaponIcon(true);
				m_pUIManager->Active_WeaponFrame(true);
				m_pUIManager->Active_WeaponActiveGuige();
				m_pUIManager->NonActive_SkillIcon();
				m_pUIManager->NonActive_SkillFrame();
				m_pUIManager->NonActive_SkillActiveGuige();
				m_pUIManager->Skill_NotPicking();
			}
			else if (m_tUIInfo.strProtoTag == "SkillButton" ||
				m_tUIInfo.strProtoTag == "SkillButtonActive")
			{
				m_bSelectButton = true;
				m_pUIManager->Select_SkillWindowButton("SkillButton", false); // 선택한 버튼을 제외하고 모두 꺼준다.
				//m_pUIManager->ActiveSkill();
				//m_pUIManager->NonActiveWeapon();
				m_pUIManager->Active_SkillIcon(true);
				m_pUIManager->Active_SkillFrame(true);
				m_pUIManager->Active_SkillActiveGuige();
				m_pUIManager->NonActive_WeaponIcon();
				m_pUIManager->NonActive_WeaponFrame();
				m_pUIManager->NonActive_WeaponActiveGuige();
				m_pUIManager->Weapon_NotPicking();
			}
		}
	}
	else
	{
		m_bSoundOk = false;
		m_bSoundOk2 = false;
	}
}

void CUI_SkillWindow_Button::Check_State(_float fTimeDelta)
{
	if (m_tUIInfo.strProtoTag == "WeaponButton" || m_tUIInfo.strProtoTag == "WeaponButtonActive")
	{
		if (m_bSelectButton == true)
		{
			if (m_fScaleX < 170.f)
				Change_SizeX((+m_fChangeScale * 1.5f));

			if (m_fScaleY < 80.f)
				Change_SizeY((+m_fChangeScale * 1.5f));
		}
		else
		{
			if (m_fScaleX > 140.f)
				Change_SizeX((-m_fChangeScale * 1.5f));

			if (m_fScaleY > 60.f)
				Change_SizeY((-m_fChangeScale * 1.5f));
		}

	}
	if (m_tUIInfo.strProtoTag == "SkillButton" || m_tUIInfo.strProtoTag == "SkillButtonActive")
	{
		if (m_bSelectButton == true)
		{
			if (m_fScaleX < 170.f)
				Change_SizeX((+m_fChangeScale * 1.5f));

			if (m_fScaleY < 80.f)
				Change_SizeY((+m_fChangeScale * 1.5f));
		}
		else
		{
			if (m_fScaleX > 140.f)
				Change_SizeX((-m_fChangeScale * 1.5f));

			if (m_fScaleY > 60.f)
				Change_SizeY((-m_fChangeScale * 1.5f));
		}
	}
}

HRESULT CUI_SkillWindow_Button::Ready_Components()
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
	if (m_tUIInfo.strProtoTag == "WeaponButton" ||
		m_tUIInfo.strProtoTag == "WeaponButtonActive")
	{
		//! For.Com_Texture2 // NonActive
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("WeaponButton"),
			TEXT("Com_Texture_SkillWindow_NonActive"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE]))))
			return E_FAIL;

		//! For.Com_Texture2 // Active
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("WeaponButtonActive"),
			TEXT("Com_Texture_SkillWindow_Active"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE]))))
			return E_FAIL;
	}
	else if (m_tUIInfo.strProtoTag == "SkillButton" ||
			 m_tUIInfo.strProtoTag == "SkillButtonActive")
	{
		//! For.Com_Texture2 // NonActive
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SkillButton"),
			TEXT("Com_Texture_SkillWindow_NonActive"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE]))))
			return E_FAIL;

		//! For.Com_Texture2 // Active
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SkillButtonActive"),
			TEXT("Com_Texture_SkillWindow_Active"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE]))))
			return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CUI_SkillWindow_Button::Bind_ShaderResources()
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

	if (m_tUIInfo.strProtoTag == "WeaponButton")
	{
		if (m_bSelectButton == true || m_bPick == true)
		{
			if (FAILED(m_pTextureCom[ACTIVE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pTextureCom[NONACTIVE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
	}


	if (m_tUIInfo.strProtoTag == "SkillButton")
	{
		if (m_bSelectButton == true || m_bPick == true)
		{
			if (FAILED(m_pTextureCom[ACTIVE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pTextureCom[NONACTIVE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}

	}
	else
	{

	}

	return S_OK;
}

json CUI_SkillWindow_Button::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_SkillWindow_Button::Load_Desc()
{

}

CUI_SkillWindow_Button* CUI_SkillWindow_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_SkillWindow_Button* pInstance = new CUI_SkillWindow_Button(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_SkillWindow_Button");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_SkillWindow_Button::Pool()
{
	return new CUI_SkillWindow_Button(*this);
}

CGameObject* CUI_SkillWindow_Button::Clone(void* pArg)
{
	CUI_SkillWindow_Button* pInstance = new CUI_SkillWindow_Button(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_SkillWindow_Button");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_SkillWindow_Button::Free()
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
