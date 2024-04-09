#include "stdafx.h"
#include "UI_SkillScrew.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "Texture.h"

CUI_SkillScrew::CUI_SkillScrew(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_SkillScrew::CUI_SkillScrew(const CUI_SkillScrew& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_SkillScrew::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_SkillScrew::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	return S_OK;
}

void CUI_SkillScrew::Priority_Tick(_float fTimeDelta)
{

}

void CUI_SkillScrew::Tick(_float fTimeDelta)
{
	//if(!m_vecAnimation.empty())
	//	m_bActive = m_vecAnimation[iFrameIndex].bDisappear;

	__super::Tick(fTimeDelta);

	if (m_bActive == true)
	{

	}
}

void CUI_SkillScrew::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();


	//if (m_tUIInfo.pParentTransformCom != nullptr &&
	//	m_tUIInfo.bParent == false)
	//}Render_UI_MRT

	//	/* Parent */
	//	_vector vPosition = m_tUIInfo.pParentTransformCom->Get_State(CTransform::STATE_POSITION);
	//	XMMATRIX ParentMat = m_tUIInfo.pParentTransformCom->Get_WorldMatrix();
	//	/* Child */
	//	XMMATRIX ChildMat = m_pTransformCom->Get_WorldMatrix();

	//	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_tUIInfo.pParentTransformCom->Get_WorldMatrix());

	//	m_pTransformCom->Set_WorldMatrix(m_WorldMatrix);
	//}

	if (m_bActive == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_SkillScrew::Render()
{
	//TODO 셰이더에게 행렬을 던져주는 행위는 반드시 셰이더의 비긴함수를 호출하기 이전에 해야한다.
	//! 그 이유는, 셰이더의 비긴함수 내에서 pPass->Apply(0, m_prContext); 코드를 수행한다.
	//! Apply 호출 후X 행렬을 던져줘도 에러는 나지 않지만, 안정성이 떨어진다.
	//! Apply 호출 후에 행렬을 던져주면, 어떤 때에는 정상적으로 수행되고, 어떤 때에는 값이 제대로 안 넘어가는 경우가 있다.


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

void CUI_SkillScrew::UI_Ready(_float fTimeDelta)
{
}

void CUI_SkillScrew::UI_Enter(_float fTimeDelta)
{
}

void CUI_SkillScrew::UI_Loop(_float fTimeDelta)
{
}

void CUI_SkillScrew::UI_Exit(_float fTimeDelta)
{
}

HRESULT CUI_SkillScrew::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//! For.Com_Texture1 // 비활성화
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_perks_hub"), // error : 테그도 잘맞고 내부 함수에서 텍스처도 잘 찾는데 nullptr로 터짐 -> m_pTextureCom을 사용은 배열식으로 사용했는데, 멤버변수로 선언할때 배열로 선언 안했음.. 
		TEXT("Com_Texture_SkillIcon_NonActive"), reinterpret_cast<CComponent**>(&m_pTextureCom[NONACTIVE]))))
		return E_FAIL;

	//! For.Com_Texture2 // 활성화
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_perks_hub_active"),
		TEXT("Com_Texture_SkillIcon_Active"), reinterpret_cast<CComponent**>(&m_pTextureCom[ACTIVE]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillScrew::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;


	/* UI_Level */
	switch (m_eUI_Level)
	{
	case Client::CUI::LEVEL0: /* => Lock/NonActive <= */
		if (FAILED(m_pTextureCom[NONACTIVE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	case Client::CUI::LEVEL1: /* => Unlock/Active <= */
		if (FAILED(m_pTextureCom[ACTIVE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	case Client::CUI::LEVEL2:
		break;
	case Client::CUI::LEVEL3:
		break;
	case Client::CUI::LEVEL4:
		break;
	case Client::CUI::LEVEL5:
		break;
	case Client::CUI::LEVEL_END:
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CUI_SkillScrew::Set_ParentTransform(CTransform* pParentTransformCom)
{
	m_tUIInfo.pParentTransformCom = pParentTransformCom;
	return S_OK;
}

void CUI_SkillScrew::Compute_OwnerCamDistance()
{
	//_vector		vPosition = m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	//m_fOwnerCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

_bool CUI_SkillScrew::In_Frustum()
{
	return false;
	//return m_pGameInstance->isIn_WorldPlanes(m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION), 2.f);
}

json CUI_SkillScrew::Save_Desc(json& out_json)
{
	// Save error : 저장을 상위 부모에서 바꿨는데 이 클래스에는 적용안했음.
	__super::Save_Desc(out_json);

	//_float fSizeX = 0.f;
	//_float fSizeY = 0.f;
	//_float fPositionX = 0.f;
	//_float fPositionY = 0.f;

	//_float fCurPosX = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
	//_float fCurPosY = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];

	//fCurPosX = fCurPosX + (_float)g_iWinSizeX * 0.5f;
	//fCurPosY = (_float)g_iWinSizeY * 0.5f - fCurPosY;

	//out_json["CloneTag"] = m_tUIInfo.strCloneTag;

	//out_json["ProtoTag"] = m_tUIInfo.strProtoTag;

	//out_json["FilePath"] = m_tUIInfo.strFilePath;

	//m_pTransformCom->Write_Json(out_json);

	return out_json;
}

void CUI_SkillScrew::Load_Desc()
{

}

CUI_SkillScrew* CUI_SkillScrew::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_SkillScrew* pInstance = new CUI_SkillScrew(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_SkillScrew");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_SkillScrew::Clone(void* pArg)
{
	CUI_SkillScrew* pInstance = new CUI_SkillScrew(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_SkillScrew");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_SkillScrew::Pool()
{
	return new CUI_SkillScrew(*this);
}

void CUI_SkillScrew::Free()
{
	__super::Free();

	for (auto& pTexture : m_pTextureCom)
	{
		if (pTexture != nullptr)
			Safe_Release(pTexture);
	}

}


