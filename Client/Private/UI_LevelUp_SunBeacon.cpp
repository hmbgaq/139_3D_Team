#include "stdafx.h"
#include "UI_LevelUp_SunBeacon.h"
#include "GameInstance.h"
#include "Json_Utility.h"

CUI_LevelUp_SunBeacon::CUI_LevelUp_SunBeacon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_LevelUp_SunBeacon::CUI_LevelUp_SunBeacon(const CUI_LevelUp_SunBeacon& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_LevelUp_SunBeacon::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_LevelUp_SunBeacon::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	m_tUIInfo.fPositionZ = 0.3f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	m_eType = UITYPE::LEVEL_UP;
	m_fChangeScale = 4.f;
	//m_fLifeTime = 5000.f;

	m_fActiveTime = 1500.f;
	m_bActive = false;
	m_fAlpha = 0.f;
	m_fLifeTime = 8000.f;
	m_fTime = (_float)GetTickCount64();

	return S_OK;
}

void CUI_LevelUp_SunBeacon::Priority_Tick(_float fTimeDelta)
{

}

void CUI_LevelUp_SunBeacon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Disappear(fTimeDelta);

	if (m_bActive == true)
	{
		if (m_bPlayAnim == true)
		{
			if (iFrameIndex == 0 && m_bSoundOk == false)
			{
				wstring strFileName = L"";
				strFileName = L"ui_level_up001.wav";

				m_pGameInstance->Play_Sound(L"UI_LevelUP", strFileName, CHANNELID::SOUND_UI_LEVELUP, 10.f);

				m_bSoundOk = true;
			}
			if (iFrameIndex == 3 && m_bSoundOk2 == false)
			{
				wstring strFileName = L"";
				strFileName = L"Player_Level_Up_v2.wav";

				//m_pGameInstance->Stop_Sound(CHANNELID::SOUND_UI_LEVELUP);
				m_pGameInstance->Play_Sound(L"UI_LevelUP", strFileName, CHANNELID::SOUND_UI_LEVELUP, 10.f);

				m_bSoundOk2 = true;
			}

		}
	}
}

void CUI_LevelUp_SunBeacon::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();

	if (m_bActive == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_LevelUp_SunBeacon::Render()
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

void CUI_LevelUp_SunBeacon::UI_Ready(_float fTimeDelta)
{
}

void CUI_LevelUp_SunBeacon::UI_Enter(_float fTimeDelta)
{
}

void CUI_LevelUp_SunBeacon::UI_Loop(_float fTimeDelta)
{
}

void CUI_LevelUp_SunBeacon::UI_Exit(_float fTimeDelta)
{
}

HRESULT CUI_LevelUp_SunBeacon::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Level_sun_element"),
		TEXT("Com_Texture_Lock"), reinterpret_cast<CComponent**>(&m_pTextureCom[SUNBEACON]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LevelUp_SunBeacon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	string TestName = m_tUIInfo.strObjectName;
	for (_int i = (_int)0; i < (_int)TEXTURE_END; ++i)
	{
		switch (i)
		{
		case CUI_LevelUp_SunBeacon::SUNBEACON:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		}
		default:
			break;
		}
	}

	return S_OK;
}

void CUI_LevelUp_SunBeacon::Check_Disappear(_float fTimeDelta)
{
	if (m_bDisappear == true)
	{
		m_bActive = Alpha_Plus(fTimeDelta);
	}
}

json CUI_LevelUp_SunBeacon::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_LevelUp_SunBeacon::Load_Desc()
{

}

CUI_LevelUp_SunBeacon* CUI_LevelUp_SunBeacon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_LevelUp_SunBeacon* pInstance = new CUI_LevelUp_SunBeacon(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_LevelUp_SunBeacon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_LevelUp_SunBeacon::Pool()
{
	return new CUI_LevelUp_SunBeacon(*this);
}

CGameObject* CUI_LevelUp_SunBeacon::Clone(void* pArg)
{
	CUI_LevelUp_SunBeacon* pInstance = new CUI_LevelUp_SunBeacon(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_LevelUp_SunBeacon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_LevelUp_SunBeacon::Free()
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
