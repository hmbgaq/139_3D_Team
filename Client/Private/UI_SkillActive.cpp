#include "stdafx.h"
#include "UI_SkillActive.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "UI_Manager.h"
#include "Data_Manager.h"

CUI_SkillActive::CUI_SkillActive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_SkillActive::CUI_SkillActive(const CUI_SkillActive& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_SkillActive::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_SkillActive::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	m_vCenter.x = 0.5;
	m_vCenter.y = 0.5;

	m_bActive = true;

	m_iMaskNum = 128;

	m_fMaxCoolTime = 2.f;
	m_fCoolTime = m_fMaxCoolTime;
	m_bCoolDown = true;

	m_fOriginScaleX = 50.f;
	m_fOriginScaleY = 50.f;

	return S_OK;
}

void CUI_SkillActive::Priority_Tick(_float fTimeDelta)
{

}

void CUI_SkillActive::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bActive == true)
	{
		if (m_pGameInstance->Key_Down(DIK_G))
		{
			m_fOriginScaleX = m_pTransformCom->Get_Scaled().x;
			m_fOriginScaleY = m_pTransformCom->Get_Scaled().y;
		}

		if (m_bMaxCoolDown == false)
		{
			// 활성화 (선택한 스킬이 있을 경우) [현재 레벨 1일 해금가능 상태일 때만]
			if (m_pGameInstance->Key_Pressing(DIK_SPACE) &&
				m_pUIManager->Get_Select_SkillLevel() != UI_LEVEL::LEVEL_END &&
				m_pUIManager->Get_Select_SkillLevel() >= UI_LEVEL::LEVEL1 &&	// 최소
				m_pUIManager->Get_Select_SkillLevel() < UI_LEVEL::LEVEL2)		// 최대
			{
  				if (m_pUIManager->Get_Select_SkillPoint() > m_pData_Manager->Get_SkillPoint()) // 요구포인트 > 가진포인트
				{
					m_pUIManager->Change_MessageBox(1); // 포인트 부족
					return;
				}


				if (m_pGameInstance->Key_Up(DIK_SPACE))
				{
					m_fAlpha = 0.f;
					m_fCoolTime = m_fMaxCoolTime;
					m_bMaxCoolDown = false;
					m_bCoolDown = true;

					m_pTransformCom->Set_Scaling(m_fOriginScaleX, m_fOriginScaleY, 0.1f);
				}

				if (m_bCoolDown == true &&
					m_bMaxCoolDown == false)
				{
					m_fCoolTime -= fTimeDelta; // 감소시킬수록 게이지가 증가됨 (텍스처가 씌워짐)
					m_iShaderNum = 5; // 원형 게이지 pass
				}

				if (m_fCoolTime <= 0.f && 
					m_pUIManager->Get_Select_SkillLevel() != UI_LEVEL::LEVEL_END && // 예외
					m_pUIManager->Get_Select_SkillLevel() >= UI_LEVEL::LEVEL1 &&	// 최소
					m_pUIManager->Get_Select_SkillLevel() < UI_LEVEL::LEVEL2)		// 최대
				{
					m_pUIManager->Select_SkillLevelUP(); // 레벨업
					m_bCoolDown = false;
					m_bMaxCoolDown = true;

					/* Sound */
					wstring strFileName = L"";
					strFileName = L"HM_UI_Screen_NewWeapon.wav";

					m_pGameInstance->Play_Sound(L"UI_Weapon", strFileName, CHANNELID::SOUND_UI_SKILLACTIVE, 13.f);

					// 활성화
					Check_SkillActive(fTimeDelta, SKILLSTATE::ACTIVE);
				}
				else
				{
					// 비활성화
					Check_SkillActive(fTimeDelta, SKILLSTATE::COOLDOWN);
				}
			}
			else
			{
				if (m_fCoolTime < m_fMaxCoolTime)
					m_fCoolTime += fTimeDelta; // 증가시킬수록 게이지가 감소됨 (텍스처가 사라짐)
			}
		}
		else
		{
			{
				if (m_fScaleX < 180.f)
					Change_SizeX((+m_fChangeScale * 1.5f));

				if (m_fScaleY < 180.f)
					Change_SizeY((+m_fChangeScale * 1.5f));

				if (m_fAlpha < 1.f)
					m_fAlpha += fTimeDelta * 2.0f;

				if (m_fAlpha >= 1.f)
				{
					m_pData_Manager->Add_SkillPoint(-m_pUIManager->Get_Select_SkillPoint()); // 스킬포인트 차감
					m_fAlpha = 0.f;
					m_fCoolTime = m_fMaxCoolTime;
					m_bMaxCoolDown = false;
					m_bCoolDown = true;

					m_pTransformCom->Set_Scaling(m_fOriginScaleX, m_fOriginScaleY, 0.1f);
				}
			}
		}

	}
}

void CUI_SkillActive::Late_Tick(_float fTimeDelta)
{
	if (m_bActive == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_SkillActive::Render()
{
	if (m_bActive == true)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		//! 이 셰이더에 0번째 패스로 그릴거야.
		m_pShaderCom->Begin(5); //! Shader_PosTex 7번 패스 = VS_MAIN,  PS_UI_HP

		//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
		m_pVIBufferCom->Bind_VIBuffers();

		//! 바인딩된 정점, 인덱스를 그려
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

void CUI_SkillActive::UI_Ready(_float fTimeDelta)
{
}

void CUI_SkillActive::UI_Enter(_float fTimeDelta)
{
}

void CUI_SkillActive::UI_Loop(_float fTimeDelta)
{
}

void CUI_SkillActive::UI_Exit(_float fTimeDelta)
{
}

HRESULT CUI_SkillActive::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//! For.Com_Texture2
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("hold_select_new"),
		TEXT("Com_Texture_Active"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE]))))
		return E_FAIL;

	//! For.Com_Distortion_Mask
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Mask"),
		TEXT("Com_Distortion_Mask"), reinterpret_cast<CComponent**>(&m_pDistortionCom[MASK]))))
		return E_FAIL;

	////! For.Com_Texture3
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototag,
	//	TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillActive::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	// error : 셰이더 파일에 있는거랑 타입 안맞았음.
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Center", &m_vCenter, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Radius", &m_fRadius, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CoolTime", &m_fCoolTime, sizeof(_float))))
		return E_FAIL;

	// 현재 쿨타임을 계산해서 던져준다.
	_float fRatio = (m_fMaxCoolTime - m_fCoolTime) / m_fMaxCoolTime;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &fRatio, sizeof(_float))))
		return E_FAIL;

	//if (FAILED(m_pTextureCom[INACTIVE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
	//	return E_FAIL;
	if (FAILED(m_pTextureCom[ACTIVE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pDistortionCom[MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_iMaskNum)))
		return E_FAIL;

	//if (FAILED(m_pTextureCom[ACTIVE]->Bind_ShaderResource(m_pShaderCom, "g_CoolDownTexture")))
	//	return E_FAIL;

	return S_OK;
}

void CUI_SkillActive::Check_SkillActive(_float fTimeDelta, SKILLSTATE eState)
{


}


json CUI_SkillActive::Save_Desc(json& out_json)
{
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_SkillActive::Load_Desc()
{

}

CUI_SkillActive* CUI_SkillActive::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_SkillActive* pInstance = new CUI_SkillActive(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_SkillActive");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_SkillActive::Pool()
{
	return new CUI_SkillActive(*this);
}

CGameObject* CUI_SkillActive::Clone(void* pArg)
{
	CUI_SkillActive* pInstance = new CUI_SkillActive(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_SkillActive");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_SkillActive::Free()
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
