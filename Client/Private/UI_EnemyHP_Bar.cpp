#include "stdafx.h"
#include "UI_EnemyHP_Bar.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "Texture.h"
#include "Character.h"

CUI_EnemyHP_Bar::CUI_EnemyHP_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_EnemyHP_Bar::CUI_EnemyHP_Bar(const CUI_EnemyHP_Bar& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_EnemyHP_Bar::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_EnemyHP_Bar::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	m_tUIInfo.fScaleX = 169.f;
	m_tUIInfo.fScaleY = 17.f;

	m_tUIInfo.fPositionZ = 0.0f;

	m_iShaderNum = 1;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	/*
	#include "Data_Manager.h" 인클루드 해주고,
	CData_Manager::GetInstance()->Get_Player(); 로 받아주고
	함수 그대로 사용하거나, 변수에 담아서 사용하기.
	해당 객체에 원하는 함수나 변수 만들어서 불러오기.
	*/

	// Setting_Owner(); 생성 후 세팅이니까 생성 하는곳에서 함수 호출해서 세팅해주기
	m_bActive = true;

	return S_OK;
}

void CUI_EnemyHP_Bar::Priority_Tick(_float fTimeDelta)
{

}

void CUI_EnemyHP_Bar::Tick(_float fTimeDelta)
{
	/* Owner HP */
	if(m_pCharacterOwner != nullptr)
		m_fCurHP = m_pCharacterOwner->Get_CurHP();

	__super::Tick(fTimeDelta);

	// 0. 회복
	if (m_fPreHP < m_fCurHP)
		m_fPreHP = m_fCurHP;

	/* 1. 모든 체력 소진 */
	if (m_fCurHP <= 0.f)
	{
		/* Dead */
		m_fCurHP = 0.f;
		Set_Dead(true);
	}

	/* 2. 현재 체력 -> 맥스 체력 초과 */
	if (m_fCurHP >= m_fMaxHP)
	{
		m_fCurHP = m_fMaxHP;
	}

	if (m_bActive == true)
	{
		m_fTimeAcc += fTimeDelta * 0.1f;

		if (m_fCurHP < m_fPreHP)
			m_bLerp = false;

		if (!m_bLerp && m_fPreHP > m_fCurHP)
		{
			m_fPreHP -= fTimeDelta * m_fVariationSpeed * (m_fMaxHP / 6.f);

			if (m_fPreHP <= m_fCurHP)
			{
				m_fPreHP = m_fCurHP;
				m_bLerp = true;
			}
		}
	}

}

void CUI_EnemyHP_Bar::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();

	if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
		return;
}

HRESULT CUI_EnemyHP_Bar::Render()
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

void CUI_EnemyHP_Bar::UI_Ready(_float fTimeDelta)
{
}

void CUI_EnemyHP_Bar::UI_Enter(_float fTimeDelta)
{
}

void CUI_EnemyHP_Bar::UI_Loop(_float fTimeDelta)
{
}

void CUI_EnemyHP_Bar::UI_Exit(_float fTimeDelta)
{
}

HRESULT CUI_EnemyHP_Bar::Ready_Components()
{
	//! For.Com_Texture1 // 흰색 바
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_defence_white"),
		TEXT("Com_Texture_WhiteBar"), reinterpret_cast<CComponent**>(&m_pTextureCom[HPBAR_WHITE]))))
		return E_FAIL;

	//! For.Com_Texture2 // 빨간색 바
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_boss_hp_bar"),
		TEXT("Com_Texture_RedBar"), reinterpret_cast<CComponent**>(&m_pTextureCom[HPBAR_RED]))))
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

HRESULT CUI_EnemyHP_Bar::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_LerpHP", &m_fPreHP, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurrentHP", &m_fCurHP, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaxHP", &m_fMaxHP, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pTextureCom[HPBAR_WHITE]->Bind_ShaderResource(m_pShaderCom, "g_HpBarWhite_Texture")))	// Hp White
		return E_FAIL;
	if (FAILED(m_pTextureCom[HPBAR_RED]->Bind_ShaderResource(m_pShaderCom, "g_HpBarRed_Texture")))		// Hp Red
		return E_FAIL;
	//if (FAILED(m_pTextureCom[HP_DECAL]->Bind_ShaderResource(m_pShaderCom, "g_HpBarDecal_Texture")))		// Hp Decal
	//	return E_FAIL;

	return S_OK;
}

void CUI_EnemyHP_Bar::Compute_OwnerCamDistance()
{
	//_vector		vPosition = m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	//m_fOwnerCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

HRESULT CUI_EnemyHP_Bar::Setting_Owner()
{
	if (m_pOwner != nullptr) // Owner 지정해줘야 합니다.
	{
		m_pCharacterOwner = dynamic_cast<CCharacter*>(m_pOwner);

		m_fCurHP = m_pCharacterOwner->Get_CurHP();
		m_fPreHP = m_fCurHP;
		m_fMaxHP = m_pCharacterOwner->Get_MaxHP();
	}
	else
	{
		m_fCurHP = 100.f;
		m_fPreHP = 100.f;
		m_fMaxHP = 100.f;
	}

	return S_OK;
}

_bool CUI_EnemyHP_Bar::In_Frustum()
{
	return false;
	//return m_pGameInstance->isIn_WorldPlanes(m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION), 2.f);
}

json CUI_EnemyHP_Bar::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);


	/* 추가정보 저장 */


	return out_json;
}

void CUI_EnemyHP_Bar::Load_Desc()
{

}

CUI_EnemyHP_Bar* CUI_EnemyHP_Bar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_EnemyHP_Bar* pInstance = new CUI_EnemyHP_Bar(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_EnemyHP_Bar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_EnemyHP_Bar::Clone(void* pArg)
{
	CUI_EnemyHP_Bar* pInstance = new CUI_EnemyHP_Bar(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_EnemyHP_Bar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_EnemyHP_Bar::Pool()
{
	return new CUI_EnemyHP_Bar(*this);
}

void CUI_EnemyHP_Bar::Free()
{
	__super::Free();

	for (auto& pTexture : m_pTextureCom)
	{
		if (pTexture != nullptr)
			Safe_Release(pTexture);
	}

}
