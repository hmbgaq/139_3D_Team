#include "stdafx.h"
#include "UI_Option_SelectBox.h"
#include "GameInstance.h"
#include "Json_Utility.h"

CUI_Option_SelectBox::CUI_Option_SelectBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI_Text(pDevice, pContext, strPrototypeTag)
{

}

CUI_Option_SelectBox::CUI_Option_SelectBox(const CUI_Option_SelectBox& rhs)
	: CUI_Text(rhs)
{
}

HRESULT CUI_Option_SelectBox::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Option_SelectBox::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Text())) //! 텍스트 세팅
		return E_FAIL;

	if (FAILED(Find_Change("Default"))) //! 텍스트 세팅
		return E_FAIL;

	return S_OK;
}

void CUI_Option_SelectBox::Priority_Tick(_float fTimeDelta)
{

}

void CUI_Option_SelectBox::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Option_SelectBox::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();


	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_Option_SelectBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//! 이 셰이더에 0번째 패스로 그릴거야.
	m_pShaderCom->Begin(0); //! Shader_PosTex 7번 패스 = VS_MAIN,  PS_UI_HP

	//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
	m_pVIBufferCom->Bind_VIBuffers();

	//! 바인딩된 정점, 인덱스를 그려
	m_pVIBufferCom->Render();

	//if (!m_bFreeMove)
	//{
	//	m_fPosX = m_pTransformCom->Get_Position().x + (g_iWinsizeX / 2.f);
	//	m_fPosY = -m_pTransformCom->Get_Position().y + (g_iWinsizeY / 2.f);
	//}

	//if (m_pTextInfo != nullptr)
	//{
	//	RenderTextWithLineBreak(m_pGameInstance->Convert_WString_To_String(m_strText), 10);
	//	m_pGameInstance->Render_Font(m_strFontTag, m_strText, _float2(m_fPosX, m_fPosY), m_vColor, m_fScale, m_vOrigin, m_fRotation);
	//}

	return S_OK;
}

void CUI_Option_SelectBox::Add_Text(string strTextKey, string strFontTag, string strText, _float fPosX, _float fPosY, _vector vColor, _float fScale, _float2 vOrigin, _float fRotation)
{
	__super::Add_Text(strTextKey, strFontTag, strText, fPosX, fPosY, vColor, fScale, vOrigin, fRotation);
}

HRESULT CUI_Option_SelectBox::Find_Change(const string& strTextTag)
{
	__super::Find_Change(strTextTag);
	return S_OK;
}

HRESULT CUI_Option_SelectBox::Ready_Components()
{
	if (FAILED(__super::Ready_Components())); // Ready : Texture / MapTexture

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_devider"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTBOX]))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_navigation"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTBOX]))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_list_selected"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTBOX]))))
		return E_FAIL;

	/* 효과가 필요한 녀석은 Map텍스쳐도 추가해주기 */
	return S_OK;
}

HRESULT CUI_Option_SelectBox::Bind_ShaderResources()
{

	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	string TestName = m_tUIInfo.strObjectName;
	for (_int i = (_int)0; i < (_int)m_eTexture_Kind; ++i)
	{
		switch (i)
		{
		case CUI_Option_SelectBox::TEXTBOX:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		}
		case CUI_Option_SelectBox::TEXTLINE:
		{
			//if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			//	return E_FAIL;

			break;
		}
		case CUI_Option_SelectBox::TEXTURE_END:
			break;
		default:
			break;
		}
	}

	return S_OK;
}

HRESULT CUI_Option_SelectBox::Ready_Text()
{
	/* 파싱 정보 받기 */
	TEXTINFO* LoadInfo = new TEXTINFO;

	/* 임의 값 (추 후 로드해서 받기) */
	LoadInfo->fPosX = g_iWinsizeX / 2.f;
	LoadInfo->fPosY = g_iWinsizeY / 2.f;
	LoadInfo->fScale = 1.f;
	LoadInfo->vOrigin.x = 0.f;
	LoadInfo->vOrigin.y = 0.f;
	LoadInfo->fRotation = 0.f;
	LoadInfo->strTextKey = "Default";
	LoadInfo->strFontTag = "Font_Arial";
	LoadInfo->strText = "Default Test OK";
	LoadInfo->vColor.m128_f32[0] = 1.f;
	LoadInfo->vColor.m128_f32[1] = 1.f;
	LoadInfo->vColor.m128_f32[2] = 1.f;
	LoadInfo->vColor.m128_f32[3] = 1.f;

	m_mapText.emplace(LoadInfo->strTextKey, LoadInfo);

	TEXTINFO* pTextInfo = nullptr;

	return S_OK;
}

void CUI_Option_SelectBox::Compute_OwnerCamDistance()
{
	//_vector		vPosition = m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	//m_fOwnerCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

_bool CUI_Option_SelectBox::In_Frustum()
{
	return false;
	//return m_pGameInstance->isIn_WorldPlanes(m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION), 2.f);
}

json CUI_Option_SelectBox::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);


	/* 추가정보 저장 */


	return out_json;
}

void CUI_Option_SelectBox::Load_Desc()
{

}

CUI_Option_SelectBox* CUI_Option_SelectBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Option_SelectBox* pInstance = new CUI_Option_SelectBox(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Option_SelectBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Option_SelectBox::Clone(void* pArg)
{
	CUI_Option_SelectBox* pInstance = new CUI_Option_SelectBox(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Option_SelectBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Option_SelectBox::Pool()
{
	return new CUI_Option_SelectBox(*this);
}

void CUI_Option_SelectBox::Free()
{
	__super::Free();
}
