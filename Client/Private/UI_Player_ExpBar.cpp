#include "stdafx.h"
#include "UI_Player_ExpBar.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "Data_Manager.h"
#include "UI_Manager.h"
#include "Player.h"

CUI_Player_ExpBar::CUI_Player_ExpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_Player_ExpBar::CUI_Player_ExpBar(const CUI_Player_ExpBar& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Player_ExpBar::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Player_ExpBar::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	m_tUIInfo.fScaleX = 169.f;
	m_tUIInfo.fScaleY = 4.f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	Check_RectPos();
	m_fOrigin_Right = (_float)m_rcUI.right;

	Set_OwnerExp();

	m_iShaderNum = 2;

	/* 무기 해금 */
	//m_pUIManager->Set_WeaponLevel("Revolver", 1);         // 리볼버

	return S_OK;
}

void CUI_Player_ExpBar::Priority_Tick(_float fTimeDelta)
{

}

void CUI_Player_ExpBar::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bActive)
	{
		if (m_pGameInstance->Key_Down(DIK_M))
		{
			m_pData_Manager->Add_CurEXP(50.f);
		}

		if (m_fPreEXP > m_pData_Manager->Get_MaxEXP())
		{
			m_pData_Manager->Add_CurLevel(1);

			m_fPreEXP = m_pData_Manager->Get_CurEXP();
		}

		// Level변동이 없을 경우
		if (m_iPreLevel == m_pData_Manager->Get_CurLevel()/*m_pPlayer->Get_Stat().iLevel*/)
		{
			// 시간 중첩
			m_fTimeAcc += fTimeDelta * 0.5f;
			// 현재 Exp를 받아온다. (키입력으로 Test중)
			//m_fCurEXP = m_fTestPlusEXP/*_float(m_pPlayer->Get_Stat().iExp)*/;

			// 현재 EXP가 PreEXP보다 높아지면
			if (m_pData_Manager->Get_CurEXP() > m_fPreEXP)
				m_bLerp = false;	// 러프 중지

			// CurEXP가 PreEXP보다 커져서 러프까지 중지됐을 경우
			if (false == m_bLerp && m_fPreEXP < m_pData_Manager->Get_CurEXP())
			{
				// 바 길이를 Ratio로 계산 (0~1)
				_float m_fRatio = m_pData_Manager->Get_CurEXP() / m_pData_Manager->Get_MaxEXP();

				// PreEXP에 중첩된 시간을 더해준다.
				m_fPreEXP += m_fTimeAcc;

				// 바 길이를 PreEXP로 계산 (0~1)
				m_fProgress = m_fPreEXP / m_pData_Manager->Get_MaxEXP();

				// =>PreEXP가 Cur랑 같거나 커졌다면 통과해서, 현재 EXP로 바 길이를 측정 후 출력. =>PreEXP가 Cur보다 아직 낮다면 PreEXP로 바 길이를 출력
				if (m_fPreEXP >= m_pData_Manager->Get_CurEXP())
				{
					m_fTimeAcc = 0.f; // 누적 시간값 초기화 (PreEXP가 CurEXP값과 같아질때까지 보간되는 값)
					m_fPreEXP = m_pData_Manager->Get_CurEXP();	// PreEXP를 CurEXP로 맞춰준다.
					m_fProgress = m_fRatio; // 바 길이를 CurEXP로 맞춰준다.
					m_bLerp = true;	// Lerp를 다시 켜준다.
				}
			}
		}
		else if (m_iPreLevel < m_pData_Manager->Get_CurLevel()/*m_pPlayer->Get_Stat().iLevel*/)
		{
			// EXPBar가 저장했던 Level이 Player의 현재 레벨과 다를 때 (작을 때)
			m_fTimeAcc += fTimeDelta * 0.5f;
			m_fPreEXP += m_fTimeAcc;

			if (m_fPreEXP >= m_pData_Manager->Get_MaxEXP())
			{
				m_fRemainEXP = m_fPreEXP - m_pData_Manager->Get_MaxEXP(); // 레벨업시 초과된 값 저장
				m_fPreEXP = m_pData_Manager->Get_MaxEXP();
			}

			m_fProgress = m_fPreEXP / m_pData_Manager->Get_MaxEXP();

			if (m_fProgress >= 1.f) // 바가 다 차면 정보를 갱신
			{
				m_iPreLevel = m_pData_Manager->Get_CurLevel();/*m_pPlayer->Get_Stat().iLevel*/;
				m_pData_Manager->NextLevl_MaxEXP(); /*m_pPlayer->Get_Stat().iMaxExp*/;
				m_pData_Manager->Set_CurEXP(m_fRemainEXP);/*m_pPlayer->Get_Stat().iExp*/; // 레벨업하고 남은 값
				m_fPreEXP = 0.f; // PreEXP는 초기화

				//// 기술 습득 안내메세지
				//{
				//	m_pUIManager->Change_TutorialText(TUTORIAL_TEXT::REWARD); // 메세지 변경
				//	m_pUIManager->Active_TutorialBox(); // Active 활성화
				//}

				// 레벨업 보상
				{
					m_pData_Manager->Add_Money(1000);
					m_pData_Manager->Add_SkillPoint(3);

					// 기본 제공
					/*if (m_tUIInfo.strUIName == "Kick")
					{
					   m_eUI_PreLevel = m_eUI_Level;
					}
					else if (m_tUIInfo.strUIName == "UpperCut")
					{
					   m_eUI_PreLevel = m_eUI_Level;
					}
					else if (m_tUIInfo.strUIName == "Heal")
					{
					   m_eUI_PreLevel = m_eUI_Level;
					}
					else if (m_tUIInfo.strUIName == "Punch")
					{
					   m_eUI_PreLevel = m_eUI_Level;
					}*/

					if (m_pData_Manager->Get_CurLevel() == 2)
					{
						/* 스킬 해금 */
						m_pUIManager->Set_SkillLevel("OneTouch", 1);      // 1터치
						m_pUIManager->Set_SkillLevel("ElectricDash", 1);   // 대쉬
						m_pUIManager->Set_SkillLevel("SuperChargeMod", 1);

						/* 무기 해금 */
						m_pUIManager->Set_WeaponLevel("Rifle_Skill1", 1);      // 라이플 스킬
					}

					if (m_pData_Manager->Get_CurLevel() == 3)
					{
						/* 스킬 해금 */
						m_pUIManager->Set_SkillLevel("TwoTouch", 1);      // 2터치
						m_pUIManager->Set_SkillLevel("DashShock", 1);      // 대쉬감전 효과
						m_pUIManager->Set_SkillLevel("TeleportPunch", 1);
						m_pUIManager->Set_SkillLevel("ElectricCord", 1);   // 전기줄 당기기
						/* 스킬 해금 */

						/* 무기 해금 */
						m_pUIManager->Set_WeaponLevel("Revolver_Skill1", 1);   // 리볼버 스킬
					}

					if (m_pData_Manager->Get_CurLevel() == 4)
					{
						/* 스킬 해금 */
						m_pUIManager->Set_SkillLevel("ThreeTouch", 1);      // 3터치
						m_pUIManager->Set_SkillLevel("ComboPunch", 1);      // 콤보펀치
						m_pUIManager->Set_SkillLevel("NPCPowerUP", 1);
						m_pUIManager->Set_SkillLevel("IncreaseEXP", 1);

					}

					if (m_pData_Manager->Get_CurLevel() == 5)
					{

						/* 스킬 해금 */
						m_pUIManager->Set_SkillLevel("IncreaseHP", 1);
						m_pUIManager->Set_SkillLevel("IncreaseEnergy", 1);
						m_pUIManager->Set_SkillLevel("RecoveryEnergy", 1);
						m_pUIManager->Set_SkillLevel("MaxHP", 1);

						/* 무기 해금 */
						m_pUIManager->Set_WeaponLevel("Shotgun", 1);         // 샷건
						/* 무기스킬 해금 */
						m_pUIManager->Set_WeaponLevel("Shotgun_Skill1", 1);      // 샷건 스킬
					}

					if (m_pData_Manager->Get_CurLevel() == 6)
					{
						/* 스킬 해금 */

					}

					if (m_pData_Manager->Get_CurLevel() == 7)
					{

					}

					if (m_pData_Manager->Get_CurLevel() == 8)
					{
						/* 스킬 해금 */
					}

					if (m_pData_Manager->Get_CurLevel() == 9)
					{

					}

					//m_pData_Manager->Set_ShowLevelBox(true);
					m_pUIManager->Active_LevelUp();
					m_fTimeAcc = 0.f;
					m_bLerp = true;
				}
			}
		}
	}
}

void CUI_Player_ExpBar::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();

	if (m_bActive)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_Player_ExpBar::Render()
{
	if (m_bActive)
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

	return S_OK;
}

void CUI_Player_ExpBar::UI_Ready(_float fTimeDelta)
{
}

void CUI_Player_ExpBar::UI_Enter(_float fTimeDelta)
{
}

void CUI_Player_ExpBar::UI_Loop(_float fTimeDelta)
{
}

void CUI_Player_ExpBar::UI_Exit(_float fTimeDelta)
{
}

HRESULT CUI_Player_ExpBar::Ready_Components()
{
	FAILED_CHECK(__super::Ready_Components()); // Ready : Texture / MapTexture

	//! For.Com_Texture1 // 경험치 바
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_exp_bar"),
		TEXT("Com_Texture_ExpBar"), reinterpret_cast<CComponent**>(&m_pTextureCom[EXPBAR]))))
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

HRESULT CUI_Player_ExpBar::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[EXPBAR]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_LoadingProgress", &m_fProgress, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CUI_Player_ExpBar::Compute_OwnerCamDistance()
{
	//_vector		vPosition = m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	//m_fOwnerCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

_bool CUI_Player_ExpBar::In_Frustum()
{
	return false;
	//return m_pGameInstance->isIn_WorldPlanes(m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION), 2.f);
}

void CUI_Player_ExpBar::Set_OwnerExp()
{
	m_fMaxEXP = 100.f;
	m_fCurEXP = 00.f;
	m_fPreEXP = m_fCurEXP;
	m_fProgress = 0.f;
}

json CUI_Player_ExpBar::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);


	/* 추가정보 저장 */


	return out_json;
}

void CUI_Player_ExpBar::Load_Desc()
{

}

CUI_Player_ExpBar* CUI_Player_ExpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Player_ExpBar* pInstance = new CUI_Player_ExpBar(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Player_ExpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Player_ExpBar::Clone(void* pArg)
{
	CUI_Player_ExpBar* pInstance = new CUI_Player_ExpBar(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Player_ExpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Player_ExpBar::Pool()
{
	return new CUI_Player_ExpBar(*this);
}

void CUI_Player_ExpBar::Free()
{
	__super::Free();

	for (auto& pTexture : m_pTextureCom)
	{
		if (pTexture != nullptr)
			Safe_Release(pTexture);
	}

}
