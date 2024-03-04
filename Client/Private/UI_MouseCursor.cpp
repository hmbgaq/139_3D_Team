#include "stdafx.h"
#include "UI_MouseCursor.h"
#include "GameInstance.h"
#include "Json_Utility.h"

CUI_MouseCursor::CUI_MouseCursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_MouseCursor::CUI_MouseCursor(const CUI_MouseCursor& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_MouseCursor::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_MouseCursor::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	m_tUIInfo.fScaleX = 5.0f;
	m_tUIInfo.fScaleY = 5.0f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	m_ptOffset = { 0, 0 };

	m_bActive = true;

	return S_OK;
}

void CUI_MouseCursor::Priority_Tick(_float fTimeDelta)
{

}

void CUI_MouseCursor::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{

		// Test : Play / Option
		if (m_pGameInstance->Key_Down(DIK_X))
		{
			if (m_eCurMouse == PLAYGAME_CURSOR)
			{
				m_eCurMouse = OPTION_CURSOR;
			}
			else
			{
				m_eCurMouse = PLAYGAME_CURSOR;
			}
		}

		if (m_eCurMouse == PLAYGAME_CURSOR)
		{
			m_pTransformCom->Set_Scaling(70.f, 70.f, 1.f);
		}
		if (m_eCurMouse == OPTION_CURSOR)
		{
			m_pTransformCom->Set_Scaling(5.f, 5.f, 1.f);
		}

		GetCursorPos(&m_ptMouse);
		ScreenToClient(g_hWnd, &m_ptMouse);  // 클라이언트 내에 마우스 포인터 가져오기 

		m_fPositionX = m_ptMouse.x + m_ptOffset.x;
		m_fPositionY = m_ptMouse.y + m_ptOffset.y;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			XMVectorSet(m_fPositionX - g_iWinSizeX * 0.5f, -(m_fPositionY - g_iWinSizeY * 0.5f), 0.f, 1.f));

		__super::Tick(fTimeDelta);
	}
}

void CUI_MouseCursor::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_MouseCursor::Render()
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

HRESULT CUI_MouseCursor::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//! For.Com_Texture 인게임 커서
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_aim_dot"),
		TEXT("Com_Texture_GamePlay_Cursor"), reinterpret_cast<CComponent**>(&m_pTextureCom[PLAYGAME_CURSOR]))))
		return E_FAIL;

	//! For.Com_Texture 옵션 커서
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_cursor"),
		TEXT("Com_Texture_Option_Cursor"), reinterpret_cast<CComponent**>(&m_pTextureCom[OPTION_CURSOR]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MouseCursor::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	switch (m_eCurMouse)
	{
	case CUI_MouseCursor::PLAYGAME_CURSOR:
	{
		if (FAILED(m_pTextureCom[PLAYGAME_CURSOR]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	}
	case CUI_MouseCursor::OPTION_CURSOR:
	{
		if (FAILED(m_pTextureCom[OPTION_CURSOR]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	}
	default:
		break;
	}

	return S_OK;
}

json CUI_MouseCursor::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_MouseCursor::Load_Desc()
{

}

CUI_MouseCursor* CUI_MouseCursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_MouseCursor* pInstance = new CUI_MouseCursor(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_MouseCursor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_MouseCursor::Pool()
{
	return new CUI_MouseCursor(*this);
}

CGameObject* CUI_MouseCursor::Clone(void* pArg)
{
	CUI_MouseCursor* pInstance = new CUI_MouseCursor(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_MouseCursor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MouseCursor::Free()
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
