#include "stdafx.h"
#include "UI_Sprite.h"
#include "GameInstance.h"
#include "Json_Utility.h"

#include "Data_Manager.h"
//#include <mfapi.h>
//#include <mfidl.h>
//#include <mfreadwrite.h>
//#include <mferror.h>

// #include <avformat.h>
//#include <libavformat/avformat.h>
//#include <libavcodec/avcodec.h>
//#include <libswresample/swresample.h>
//#include <libswscale/swscale.h>
//#include <libavutil/avutil.h>

CUI_Sprite::CUI_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_Sprite::CUI_Sprite(const CUI_Sprite& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Sprite::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Sprite::Initialize(void* pArg)
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

void CUI_Sprite::Priority_Tick(_float fTimeDelta)
{

}

void CUI_Sprite::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_N))
		m_fFrameChangeTime -= 5.f;
	if(m_pGameInstance->Key_Down(DIK_M))
		m_fFrameChangeTime += 5.f;
	if (m_pGameInstance->Key_Down(DIK_H))
		Start_Setting();

	if (m_Time + m_fFrameChangeTime < GetTickCount64())
	{
		++m_iCurrentFrame;

		// MainStart Finish -> MainLoop Change
		if (m_iCurrentFrame >= m_iMainStart_MaxFrame && m_bMainStart_Finish == false)
		{// Start
			/* Reset (Change) */
			m_iCurrentFrame = 0;
			m_bMainStart_Finish = true;
			m_bMainLoop_Finish = false;
		}
		if (m_iCurrentFrame >= m_iMainLoop_MaxFrame && m_bMainLoop_Finish == false)
		{// Loop
			/* Reset (Loop) */
			m_iCurrentFrame = 0;
		}

		m_Time = GetTickCount64();
	}
}

void CUI_Sprite::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
		return;
}

HRESULT CUI_Sprite::Render()
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

void CUI_Sprite::UI_Ready(_float fTimeDelta)
{
}

void CUI_Sprite::UI_Enter(_float fTimeDelta)
{
}

void CUI_Sprite::UI_Loop(_float fTimeDelta)
{
}

void CUI_Sprite::UI_Exit(_float fTimeDelta)
{
}

void CUI_Sprite::Start_Setting()
{
	m_pTransformCom->Set_Scaling(g_iWinSizeX, g_iWinSizeY, 1.f); // Window Size
	m_pTransformCom->Set_Position(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f)); // Center Position

	m_bMainStart_Finish = false; // Start -> Go
	m_bMainLoop_Finish = true;	 // Loop -> Wait

	m_iShaderNum = 0;			 // Defualt

	m_iMainStart_MaxFrame = 729; // StartMax
	m_iMainLoop_MaxFrame = 341;	 // LoopMax

	m_fFrameChangeTime = 20.f;	 // FrameSpeed
}

HRESULT CUI_Sprite::Ready_Components()
{
	_uint iLevel = 0;
	if (m_bTool == true)
		iLevel = (_uint)LEVEL_TOOL;
	else
		iLevel = (_uint)LEVEL_LOGO;

	//! For.Com_Texture // MainStart (LogoLevel)
	if (FAILED(__super::Add_Component(iLevel, TEXT("MainMenu_Start"),
		TEXT("Com_Texture_MainStart"), reinterpret_cast<CComponent**>(&m_pTextureCom[MAINMENU_START]))))
		return E_FAIL;

	//! For.Com_Texture // MainLoop (LogoLevel)
	if (FAILED(__super::Add_Component(iLevel, TEXT("MainMenu_Loop"),
		TEXT("Com_Texture_MainLoop"), reinterpret_cast<CComponent**>(&m_pTextureCom[MAINMENU_LOOP]))))
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

HRESULT CUI_Sprite::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (m_bMainStart_Finish == false)
	{
		if (FAILED(m_pTextureCom[MAINMENU_START]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iCurrentFrame)))	// MainStart
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom[MAINMENU_LOOP]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iCurrentFrame)))	// MainLoop
			return E_FAIL;
	}

	return S_OK;
}

json CUI_Sprite::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);


	/* 추가정보 저장 */


	return out_json;
}

void CUI_Sprite::Load_Desc()
{

}

CUI_Sprite* CUI_Sprite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Sprite* pInstance = new CUI_Sprite(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Sprite");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Sprite::Clone(void* pArg)
{
	CUI_Sprite* pInstance = new CUI_Sprite(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Sprite");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Sprite::Pool()
{
	return new CUI_Sprite(*this);
}

void CUI_Sprite::Free()
{
	__super::Free();

	for (auto& pTexture : m_pTextureCom)
	{
		if (pTexture != nullptr)
			Safe_Release(pTexture);
	}

}
