#include "stdafx.h"
#include "UI_Player_Skill_Icon.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "UI_Manager.h"

CUI_Player_Skill_Icon::CUI_Player_Skill_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_Player_Skill_Icon::CUI_Player_Skill_Icon(const CUI_Player_Skill_Icon& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Player_Skill_Icon::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Player_Skill_Icon::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	m_tUIInfo.fScaleX = 55.0f;
	m_tUIInfo.fScaleY = 55.0f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	/* Left */
	if (m_tUIInfo.strUIName == "LeftHUD_Top")
	{
		m_bUnlock = true;
	}
	else if (m_tUIInfo.strUIName == "LeftHUD_Right")
	{
		// Test
		m_bUnlock = false/*true*/;
	}
	else if (m_tUIInfo.strUIName == "LeftHUD_Bottom")
	{
		m_bUnlock = true;
	}
	else if (m_tUIInfo.strUIName == "LeftHUD_Left")
	{
		m_bUnlock = true;
	}

	/* Right */
	if (m_tUIInfo.strUIName == "RightHUD_Top")
	{
		m_bUnlock = true;
	}
	else if (m_tUIInfo.strUIName == "RightHUD_Right")
	{
		m_bUnlock = true;
	}
	else if (m_tUIInfo.strUIName == "RightHUD_Bottom")
	{
		m_bUnlock = true;
	}
	else if (m_tUIInfo.strUIName == "RightHUD_Left")
	{
		m_bUnlock = true;
	}


	return S_OK;
}

void CUI_Player_Skill_Icon::Priority_Tick(_float fTimeDelta)
{

}

void CUI_Player_Skill_Icon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bActive == true)
	{
		/* Left */
		if (m_tUIInfo.strUIName == "LeftHUD_Top")
		{
			if (m_bUnlock == true) // 스킬이 해금 됐을 경우
			{
				if (m_pUIManager->Get_LeftHUD_MaxCoolTime("LeftHUD_Top") > m_pUIManager->Get_LeftHUD_CurrentCoolTime("LeftHUD_Top"))
					m_eSkillState = SKILLSTATE::COOLDOWN;
				else
					m_eSkillState = SKILLSTATE::ACTIVE;
			}
			else // 해금 안됐을 경우
			{
				// 현재 쿨타임 0으로 만들기
				m_pUIManager->Change_LeftHUD_CurrentCoolTime("LeftHUD_Top", m_pUIManager->Get_LeftHUD_MaxCoolTime("LeftHUD_Top"));
			}
		}
		else if (m_tUIInfo.strUIName == "LeftHUD_Right")
		{
			if (m_bUnlock == true) // 스킬이 해금 됐을 경우
			{
				if (m_pUIManager->Get_LeftHUD_MaxCoolTime("LeftHUD_Right") > m_pUIManager->Get_LeftHUD_CurrentCoolTime("LeftHUD_Right"))
					m_eSkillState = SKILLSTATE::COOLDOWN;
				else
					m_eSkillState = SKILLSTATE::ACTIVE;
			}
			else // 해금 안됐을 경우
			{
				// 현재 쿨타임 0으로 만들기
				m_pUIManager->Change_LeftHUD_CurrentCoolTime("LeftHUD_Right", m_pUIManager->Get_LeftHUD_MaxCoolTime("LeftHUD_Right"));
			}
		}
		else if (m_tUIInfo.strUIName == "LeftHUD_Bottom")
		{
			if (m_bUnlock == true) // 스킬이 해금 됐을 경우
			{
				if (m_pUIManager->Get_LeftHUD_MaxCoolTime("LeftHUD_Bottom") > m_pUIManager->Get_LeftHUD_CurrentCoolTime("LeftHUD_Bottom"))
					m_eSkillState = SKILLSTATE::COOLDOWN;
				else
					m_eSkillState = SKILLSTATE::ACTIVE;
			}
			else // 해금 안됐을 경우
			{
				// 현재 쿨타임 0으로 만들기
				m_pUIManager->Change_LeftHUD_CurrentCoolTime("LeftHUD_Bottom", m_pUIManager->Get_LeftHUD_MaxCoolTime("LeftHUD_Bottom"));
			}
		}
		else if (m_tUIInfo.strUIName == "LeftHUD_Left")
		{
			if (m_bUnlock == true) // 스킬이 해금 됐을 경우
			{
				if (m_pUIManager->Get_LeftHUD_MaxCoolTime("LeftHUD_Left") > m_pUIManager->Get_LeftHUD_CurrentCoolTime("LeftHUD_Left"))
					m_eSkillState = SKILLSTATE::COOLDOWN;
				else
					m_eSkillState = SKILLSTATE::ACTIVE;
			}
			else // 해금 안됐을 경우
			{
				// 현재 쿨타임 0으로 만들기
				m_pUIManager->Change_LeftHUD_CurrentCoolTime("LeftHUD_Left", m_pUIManager->Get_LeftHUD_MaxCoolTime("LeftHUD_Left"));
			}
		}

		/* Right */
		if (m_tUIInfo.strUIName == "RightHUD_Top")
		{
			if (m_bUnlock == true) // 스킬이 해금 됐을 경우
			{
				if (m_pUIManager->Get_RightHUD_MaxCoolTime("RightHUD_Top") > m_pUIManager->Get_RightHUD_CurrentCoolTime("RightHUD_Top"))
					m_eSkillState = SKILLSTATE::COOLDOWN;
				else
					m_eSkillState = SKILLSTATE::ACTIVE;
			}
			else // 해금 안됐을 경우
			{
				// 현재 쿨타임 0으로 만들기
				m_pUIManager->Change_RightHUD_CurrentCoolTime("RightHUD_Top", m_pUIManager->Get_RightHUD_MaxCoolTime("RightHUD_Top"));
			}
		}
		else if (m_tUIInfo.strUIName == "RightHUD_Right")
		{
			if (m_bUnlock == true) // 스킬이 해금 됐을 경우
			{
				if (m_pUIManager->Get_RightHUD_MaxCoolTime("RightHUD_Right") > m_pUIManager->Get_RightHUD_CurrentCoolTime("RightHUD_Right"))
					m_eSkillState = SKILLSTATE::COOLDOWN;
				else
					m_eSkillState = SKILLSTATE::ACTIVE;
			}
			else // 해금 안됐을 경우
			{
				// 현재 쿨타임 0으로 만들기
				m_pUIManager->Change_RightHUD_CurrentCoolTime("RightHUD_Right", m_pUIManager->Get_RightHUD_MaxCoolTime("RightHUD_Right") + 3.f);
			}
		}
		else if (m_tUIInfo.strUIName == "RightHUD_Bottom")
		{
			if (m_bUnlock == true) // 스킬이 해금 됐을 경우
			{
				if (m_pUIManager->Get_RightHUD_MaxCoolTime("RightHUD_Bottom") > m_pUIManager->Get_RightHUD_CurrentCoolTime("RightHUD_Bottom"))
					m_eSkillState = SKILLSTATE::COOLDOWN;
				else
					m_eSkillState = SKILLSTATE::ACTIVE;
			}
			else // 해금 안됐을 경우
			{
				// 현재 쿨타임 0으로 만들기
				m_pUIManager->Change_RightHUD_CurrentCoolTime("RightHUD_Bottom", m_pUIManager->Get_RightHUD_MaxCoolTime("RightHUD_Bottom"));
			}
		}
		else if (m_tUIInfo.strUIName == "RightHUD_Left")
		{
			if (m_bUnlock == true) // 스킬이 해금 됐을 경우
			{
				if (m_pUIManager->Get_RightHUD_MaxCoolTime("RightHUD_Left") > m_pUIManager->Get_RightHUD_CurrentCoolTime("RightHUD_Left"))
					m_eSkillState = SKILLSTATE::COOLDOWN;
				else
					m_eSkillState = SKILLSTATE::ACTIVE;
			}
			else // 해금 안됐을 경우
			{
				// 현재 쿨타임 0으로 만들기
				m_pUIManager->Change_RightHUD_CurrentCoolTime("RightHUD_Left", m_pUIManager->Get_RightHUD_MaxCoolTime("RightHUD_Left"));
			}
		}
	}

}

void CUI_Player_Skill_Icon::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();

	if (m_bActive == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_Player_Skill_Icon::Render()
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

void CUI_Player_Skill_Icon::UI_Ready(_float fTimeDelta)
{
}

void CUI_Player_Skill_Icon::UI_Enter(_float fTimeDelta)
{
}

void CUI_Player_Skill_Icon::UI_Loop(_float fTimeDelta)
{
}

void CUI_Player_Skill_Icon::UI_Exit(_float fTimeDelta)
{
}

HRESULT CUI_Player_Skill_Icon::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	
	//! For.Com_Texture1 // 잠김
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_icons_hud_locked"),
		TEXT("Com_Texture_Lock"), reinterpret_cast<CComponent**>(&m_pTextureCom[ICON_LOCK]))))
		return E_FAIL;

#pragma region 아이콘은 텍스처를 골라서 넣을 수 있게 해줘야한다.
	/* 첫 Create는 아무거나 기본 아이콘으로 Initialize에서 테그를 지정해주고, 선택된 텍스처를 파싱하면 파싱된 텍스처를 사용하게 해주자. */
	wstring strPrototag;
	m_pGameInstance->String_To_WString(m_tUIInfo.strProtoTag, strPrototag);
	wstring strPrototagCooldown;
	
	// 부분 문자열 검색
	size_t found = strPrototag.find(L"ui_icon");

	if (strPrototag != L"ui_icons_kick" &&
		strPrototag != L"ui_icons_amped_post_combo_blast"&&
		strPrototag != L"ui_icons_zappem_all")
	{
		// 발견된 경우
		if (found != std::wstring::npos)
		{
			strPrototagCooldown = strPrototag + L"_cooldown";

			//! For.Com_Texture2 // 쿨타임	
			if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototagCooldown,
				TEXT("Com_Texture_Cooldown"), reinterpret_cast<CComponent**>(&m_pTextureCom[ICON_COOLDOWN]))))
				return E_FAIL;
		}
		else // 발견되지 않은 경우
		{

		}
		//! For.Com_Texture2 // 활성화
		if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototag,
			TEXT("Com_Texture_Active"), reinterpret_cast<CComponent**>(&m_pTextureCom[ICON_ACTIVE]))))
			return E_FAIL;
	}
	else
	{
		//! For.Com_Texture1 // 쿨타임
		if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototag,
			TEXT("Com_Texture_Cooldown"), reinterpret_cast<CComponent**>(&m_pTextureCom[ICON_COOLDOWN]))))
			return E_FAIL;

		strPrototag = strPrototag + L"_active";

		//! For.Com_Texture2 // 활성화
		if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototag,
			TEXT("Com_Texture_Active"), reinterpret_cast<CComponent**>(&m_pTextureCom[ICON_ACTIVE]))))
			return E_FAIL;
	}
	

#pragma endregion

	return S_OK;
}

HRESULT CUI_Player_Skill_Icon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	/* 해금을 안했을 경우 상태를 Lock으로 고정한다. */
	if (m_bUnlock == false)
		m_eSkillState = SKILLSTATE::LOCK;

	switch (m_eSkillState)
	{
		case Client::SKILLSTATE::LOCK:
			if (FAILED(m_pTextureCom[ICON_LOCK]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		case Client::SKILLSTATE::COOLDOWN:
			if (FAILED(m_pTextureCom[ICON_COOLDOWN]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		case Client::SKILLSTATE::ACTIVE:
			if (FAILED(m_pTextureCom[ICON_ACTIVE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		case Client::SKILLSTATE::SKILL_END:
			break;
		default:
			break;
	}

	return S_OK;
}

json CUI_Player_Skill_Icon::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_Player_Skill_Icon::Load_Desc()
{

}

CUI_Player_Skill_Icon* CUI_Player_Skill_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Player_Skill_Icon* pInstance = new CUI_Player_Skill_Icon(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Player_Skill_Icon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Player_Skill_Icon::Pool()
{
	return new CUI_Player_Skill_Icon(*this);
}

CGameObject* CUI_Player_Skill_Icon::Clone(void* pArg)
{
	CUI_Player_Skill_Icon* pInstance = new CUI_Player_Skill_Icon(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Player_Skill_Icon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Player_Skill_Icon::Free()
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
