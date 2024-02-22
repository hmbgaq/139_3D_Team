#include "stdafx.h"
#include "UI_Blood_Veins.h"
#include "GameInstance.h"
#include "Json_Utility.h"

CUI_Blood_Veins::CUI_Blood_Veins(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_Blood_Veins::CUI_Blood_Veins(const CUI_Blood_Veins& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Blood_Veins::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Blood_Veins::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	return S_OK;
}

void CUI_Blood_Veins::Priority_Tick(_float fTimeDelta)
{

}

void CUI_Blood_Veins::Tick(_float fTimeDelta)
{

}

void CUI_Blood_Veins::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();

	__super::Tick(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_Blood_Veins::Render()
{
	//TODO 셰이더에게 행렬을 던져주는 행위는 반드시 셰이더의 비긴함수를 호출하기 이전에 해야한다.
	//! 그 이유는, 셰이더의 비긴함수 내에서 pPass->Apply(0, m_prContext); 코드를 수행한다.
	//! Apply 호출 후에 행렬을 던져줘도 에러는 나지 않지만, 안정성이 떨어진다.
	//! Apply 호출 후에 행렬을 던져주면, 어떤 때에는 정상적으로 수행되고, 어떤 때에는 값이 제대로 안 넘어가는 경우가 있다.

	//switch (m_tUIInfo.eUIType)
	//{
	//case CUI_Blood_Veins::SMALL:
	//case CUI_Blood_Veins::MID:
	//case CUI_Blood_Veins::LARGE:
	//case CUI_Blood_Veins::SIDE:
	//{
	//	///* 월드상의 몬스터 위치로 계산된 UI를 카메라 절두체 안에 들어왔을 경우에만 표시하기 위함 */
	//	//if (m_fOwnerCamDistance > 40.f || false == In_Frustum())
	//	//{
	//	//	// m_pGameInstance->Get_CamDir();
	//	//	return E_FAIL;
	//	//}
	//	//__super::SetUp_WorldToScreen(m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION));

	//	//__super::SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY, m_fDefaultScale * m_fScaleOffsetX, m_fDefaultScale * m_fScaleOffsetY);


	//	//__super::SetUp_BillBoarding();
	//	break;
	//}
	//case CUI_Blood_Veins::BOSS:
	//case CUI_Blood_Veins::NONE:
	//	break;
	//default:
	//	break;
	//}

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

HRESULT CUI_Blood_Veins::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//! For.Com_Texture_1
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_health_bar_bg_full"), // HP_Bar_Red
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Blood_Veins::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	for (auto& iter : m_pTextureCom)
	{
		if (FAILED(iter->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
	}

	return S_OK;
}

void CUI_Blood_Veins::Compute_OwnerCamDistance()
{
	//_vector		vPosition = m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	//m_fOwnerCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

_bool CUI_Blood_Veins::In_Frustum()
{
	return false;
	//return m_pGameInstance->isIn_WorldPlanes(m_tUIInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION), 2.f);
}

json CUI_Blood_Veins::Save_Desc(json& out_json)
{
	_float fSizeX = 0.f;
	_float fSizeY = 0.f;
	_float fPositionX = 0.f;
	_float fPositionY = 0.f;

	_float fCurPosX = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
	_float fCurPosY = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];

	fCurPosX = fCurPosX + (_float)g_iWinSizeX * 0.5f;
	fCurPosY = (_float)g_iWinSizeY * 0.5f - fCurPosY;

	out_json["CloneTag"] = m_tUIInfo.strCloneTag;

	out_json["ProtoTag"] = m_tUIInfo.strProtoTag;

	out_json["FilePath"] = m_tUIInfo.strFilePath;

	m_pTransformCom->Write_Json(out_json);

	return out_json;
}

void CUI_Blood_Veins::Load_Desc()
{

}

CUI_Blood_Veins* CUI_Blood_Veins::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Blood_Veins* pInstance = new CUI_Blood_Veins(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Blood_Veins");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Blood_Veins::Pool()
{
	return new CUI_Blood_Veins(*this);
}

CGameObject* CUI_Blood_Veins::Clone(void* pArg)
{
	CUI_Blood_Veins* pInstance = new CUI_Blood_Veins(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Blood_Veins");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Blood_Veins::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	for (auto& pTexture : m_pTextureCom)
	{
		Safe_Release(pTexture);
	}

}
