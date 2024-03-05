#include "stdafx.h"
#include "UI_TextBox.h"
#include "GameInstance.h"
#include "Json_Utility.h"

CUI_TextBox::CUI_TextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_TextBox::CUI_TextBox(const CUI_TextBox& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_TextBox::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_TextBox::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_TextBox::Priority_Tick(_float fTimeDelta)
{

}

void CUI_TextBox::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_TextBox::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();


	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_TextBox::Render()
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

HRESULT CUI_TextBox::Ready_Components()
{
	FAILED_CHECK(__super::Ready_Components()); // Ready : Texture / MapTexture

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_health_bar_damagel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[HPBAR_WHITE]))))
		return E_FAIL;

	/* 효과가 필요한 녀석은 Map텍스쳐도 추가해주기 */
	return S_OK;
}

HRESULT CUI_TextBox::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	string TestName = m_tUIInfo.strObjectName;
	for (_int i = (_int)0; i < (_int)m_eTexture_Kind; ++i)
	{
		switch (i)
		{
		case CUI_TextBox::HPBAR_WHITE:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		}
		case CUI_TextBox::HPBAR_RED:
		{
			//if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			//	return E_FAIL;

			break;
		}
		case CUI_TextBox::HP_DECAL:
		{
			//if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			//	return E_FAIL;
			break;
		}
		case CUI_TextBox::TEXTURE_END:
			break;
		default:
			break;
		}
	}

	return S_OK;
}

void CUI_TextBox::Compute_OwnerCamDistance()
{
	//_vector		vPosition = m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	//m_fOwnerCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

_bool CUI_TextBox::In_Frustum()
{
	return false;
	//return m_pGameInstance->isIn_WorldPlanes(m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION), 2.f);
}

json CUI_TextBox::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);


	/* 추가정보 저장 */


	return out_json;
}

void CUI_TextBox::Load_Desc()
{

}

CUI_TextBox* CUI_TextBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_TextBox* pInstance = new CUI_TextBox(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_TextBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_TextBox::Clone(void* pArg)
{
	CUI_TextBox* pInstance = new CUI_TextBox(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_TextBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_TextBox::Pool()
{
	return new CUI_TextBox(*this);
}

void CUI_TextBox::Free()
{
	__super::Free();

	for (auto& pTexture : m_pTextureCom)
	{
		if (pTexture != nullptr)
			Safe_Release(pTexture);
	}

}
