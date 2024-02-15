#include "stdafx.h"
#include "UI_Anything.h"
#include "GameInstance.h"

CUI_Anything::CUI_Anything(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Base(pDevice, pContext)
{

}

CUI_Anything::CUI_Anything(const CUI_Anything& rhs)
	: CUI_Base(rhs)
{
}

HRESULT CUI_Anything::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Anything::Initialize(void* pArg)
{
	m_tInfo = *(UIANYTHING*)pArg;
	m_tInfo.bFrame = true;
	m_isEnable = m_tInfo.bEnable;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg))) //!  트랜스폼 셋팅, m_tInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	//m_fCurrentHp = m_tInfo.pOwnerStatus->fCurrentHp;
	//m_fPrevHp = m_fCurrentHp;
	//m_tInfo.fCrntHPUV = m_tInfo.pOwnerStatus->fCurrentHp / m_tInfo.pOwnerStatus->fMaxHp;

	// Test
	m_fCurrentHp = 5.f;
	m_fPrevHp = 5.f;
	//m_tInfo.fCrntHPUV = m_tInfo.pOwnerStatus->fCurrentHp / m_tInfo.pOwnerStatus->fMaxHp;

	SetUp_UV(0);


	return S_OK;
}

void CUI_Anything::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Anything::Tick(_float fTimeDelta)
{


	/*m_tInfo.fCrntHPUV = m_tInfo.pOwnerStatus->fCurrentHp / m_tInfo.pOwnerStatus->fMaxHp;*/

	//if (m_tInfo.fCrntHPUV <= 0.0)
	//{
	//	m_tInfo.fCrntHPUV = 0.0;

	//	Set_Dead(true);
	//}


}

void CUI_Anything::Late_Tick(_float fTimeDelta)
{
	if (m_tUIInfo.bWorldUI == true)
		Compute_OwnerCamDistance();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_Anything::Render()
{
	//TODO 셰이더에게 행렬을 던져주는 행위는 반드시 셰이더의 비긴함수를 호출하기 이전에 해야한다.
	//! 그 이유는, 셰이더의 비긴함수 내에서 pPass->Apply(0, m_prContext); 코드를 수행한다.
	//! Apply 호출 후에 행렬을 던져줘도 에러는 나지 않지만, 안정성이 떨어진다.
	//! Apply 호출 후에 행렬을 던져주면, 어떤 때에는 정상적으로 수행되고, 어떤 때에는 값이 제대로 안 넘어가는 경우가 있다.

	if (false == m_isEnable)
		return E_FAIL;


	switch (m_tInfo.eMonsterType)
	{
	case CUI_Anything::SMALL:
	case CUI_Anything::MID:
	case CUI_Anything::LARGE:
	case CUI_Anything::SIDE:
	{
		///* 월드상의 몬스터 위치로 계산된 UI를 카메라 절두체 안에 들어왔을 경우에만 표시하기 위함 */
		//if (m_fOwnerCamDistance > 40.f || false == In_Frustum())
		//{
		//	// m_pGameInstance->Get_CamDir();
		//	return E_FAIL;
		//}
		//__super::SetUp_WorldToScreen(m_tInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION));

		//__super::SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY, m_fDefaultScale * m_fScaleOffsetX, m_fDefaultScale * m_fScaleOffsetY);


		//__super::SetUp_BillBoarding();
		break;
	}
	case CUI_Anything::BOSS:
	case CUI_Anything::NONE:
		break;
	default:
		break;
	}

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

HRESULT CUI_Anything::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tInfo.strProtoTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Anything::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

void CUI_Anything::Compute_OwnerCamDistance()
{
	_vector		vPosition = m_tInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	m_fOwnerCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

_bool CUI_Anything::In_Frustum()
{
	return m_pGameInstance->isIn_WorldPlanes(m_tInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION), 2.f);
}

CUI_Anything* CUI_Anything::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Anything* pInstance = new CUI_Anything(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Anything");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Anything::Clone(void* pArg)
{
	CUI_Anything* pInstance = new CUI_Anything(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Anything");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Anything::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

}
