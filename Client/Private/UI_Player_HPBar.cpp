#include "stdafx.h"
#include "UI_Player_HPBar.h"
#include "GameInstance.h"
#include "Json_Utility.h"

#include "Data_Manager.h"

CUI_Player_HPBar::CUI_Player_HPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_Player_HPBar::CUI_Player_HPBar(const CUI_Player_HPBar& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Player_HPBar::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Player_HPBar::Initialize(void* pArg)
{
	if(pArg != nullptr)
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

	Set_OwnerHp();

	return S_OK;
}

void CUI_Player_HPBar::Priority_Tick(_float fTimeDelta)
{

}

void CUI_Player_HPBar::Tick(_float fTimeDelta)
{
	m_fCurHP = m_pData_Manager->Get_CurHP();
	m_fMaxHP = m_pData_Manager->Get_MaxHP();

	//if (m_pGameInstance->Key_Down(DIK_Z))
	//{
		//m_bActive = true;
	//}

	//if (m_pGameInstance->Key_Down(DIK_Q)) // 피격
	//{
	//	m_pData_Manager->Add_CurHP(-10.f);
	//}

	//if (m_pGameInstance->Key_Down(DIK_E)) // 회복
	//{
	//	m_pData_Manager->Add_CurHP(10.f);
	//	m_fPreHP += 10.f;
	//}

	__super::Tick(fTimeDelta);

	// 회복
	if (m_fPreHP < m_fCurHP)
		m_fPreHP = m_fCurHP;

	m_pData_Manager->Limit_HP();

	if (m_bActive)
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

void CUI_Player_HPBar::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();

	if (m_bActive)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_Player_HPBar::Render()
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

void CUI_Player_HPBar::UI_Ready(_float fTimeDelta)
{
}

void CUI_Player_HPBar::UI_Enter(_float fTimeDelta)
{
}

void CUI_Player_HPBar::UI_Loop(_float fTimeDelta)
{
}

void CUI_Player_HPBar::UI_Exit(_float fTimeDelta)
{
}

HRESULT CUI_Player_HPBar::Ready_Components()
{
	//if(FAILED(__super::Ready_Components())); // Ready : Texture / MapTexture
	//	return E_FAIL;

	//! For.Com_Texture1 // 흰색 바
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_health_bar_damagel"),
		TEXT("Com_Texture_WhiteBar"), reinterpret_cast<CComponent**>(&m_pTextureCom[HPBAR_WHITE]))))
		return E_FAIL;

	//! For.Com_Texture2 // 빨간색 바
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_health_bar_bg_full"),
		TEXT("Com_Texture_RedBar"), reinterpret_cast<CComponent**>(&m_pTextureCom[HPBAR_RED]))))
		return E_FAIL;
	//
	////! For.Com_Texture3 // 표시선
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("UI_PlayersHP_decal"),
	//	TEXT("Com_Texture_Decal"), reinterpret_cast<CComponent**>(&m_pTextureCom[HP_DECAL]))))
	//	return E_FAIL;

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

HRESULT CUI_Player_HPBar::Bind_ShaderResources()
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

void CUI_Player_HPBar::Compute_OwnerCamDistance()
{
	//_vector		vPosition = m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	//m_fOwnerCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

_bool CUI_Player_HPBar::In_Frustum()
{
	return false;
	//return m_pGameInstance->isIn_WorldPlanes(m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION), 2.f);
}

void CUI_Player_HPBar::Set_OwnerHp(/*CPlayer pPlayer*/)
{
	// ex)
	//if (nullptr == pPlayer)
	//	return;

	//m_pOwner = pPlayer;

	//CPlayer::PLAYER_STAT StatDesc = {};
	//ZeroMemory(&StatDesc, sizeof(CPlayer::PLAYER_STAT));

	//memcpy(&StatDesc, &(m_pOwner->Get_Stat()), sizeof(CPlayer::PLAYER_STAT));

	//// Texture 선택 예시
	//if (TEXT("Texture Name1") == m_pOwner->Get_ObjectTag())
	//{

	//}
	//else if (TEXT("Texture Name2") == m_pOwner->Get_ObjectTag())
	//{

	//}
	//else if (TEXT("Texture Name3") == m_pOwner->Get_ObjectTag())
	//{

	//}

	//m_fMaxHP = StatDesc.fMaxHp;
	//m_fCurHP = StatDesc.fHp;
	//m_fPreHP = m_fCurHP;

	
	m_fMaxHP = m_pData_Manager->Get_CurHP();
	m_fCurHP = m_fMaxHP;
	m_fPreHP = m_fCurHP;
}

json CUI_Player_HPBar::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	
	/* 추가정보 저장 */


	return out_json;
}

void CUI_Player_HPBar::Load_Desc()
{

}

CUI_Player_HPBar* CUI_Player_HPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Player_HPBar* pInstance = new CUI_Player_HPBar(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Player_HPBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Player_HPBar::Clone(void* pArg)
{
	CUI_Player_HPBar* pInstance = new CUI_Player_HPBar(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Player_HPBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Player_HPBar::Pool()
{
	return new CUI_Player_HPBar(*this);
}

void CUI_Player_HPBar::Free()
{
	__super::Free();

	for (auto& pTexture : m_pTextureCom)
	{
		if (pTexture != nullptr)
			Safe_Release(pTexture);
	}

}
