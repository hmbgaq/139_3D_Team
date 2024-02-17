#include "stdafx.h"
#include "UI_MonsterHPFrame.h"
#include "GameInstance.h"

CUI_MonsterHpFrame::CUI_MonsterHpFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Base(pDevice, pContext)
{

}

CUI_MonsterHpFrame::CUI_MonsterHpFrame(const CUI_MonsterHpFrame& rhs)
	: CUI_Base(rhs)
{
}

HRESULT CUI_MonsterHpFrame::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_MonsterHpFrame::Initialize(void* pArg)
{
	m_tInfo = *(MONSTER_FRAME*)pArg;
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

void CUI_MonsterHpFrame::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MonsterHpFrame::Tick(_float fTimeDelta)
{


	/*m_tInfo.fCrntHPUV = m_tInfo.pOwnerStatus->fCurrentHp / m_tInfo.pOwnerStatus->fMaxHp;*/

	/* UI생성하자마자 사라진 이유 :UV값이 0으로 들어와서 계속 생기자마자 삭제한거였음 */
	//if (m_tInfo.fCrntHPUV <= 0.0)
	//{
	//	m_tInfo.fCrntHPUV = 0.0;

	//	Set_Dead(true);
	//}


}

void CUI_MonsterHpFrame::Late_Tick(_float fTimeDelta)
{
	if(m_tUIInfo.bWorldUI == true)
	Compute_OwnerCamDistance();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_MonsterHpFrame::Render()
{
	//TODO 셰이더에게 행렬을 던져주는 행위는 반드시 셰이더의 비긴함수를 호출하기 이전에 해야한다.
	//! 그 이유는, 셰이더의 비긴함수 내에서 pPass->Apply(0, m_prContext); 코드를 수행한다.
	//! Apply 호출 후에 행렬을 던져줘도 에러는 나지 않지만, 안정성이 떨어진다.
	//! Apply 호출 후에 행렬을 던져주면, 어떤 때에는 정상적으로 수행되고, 어떤 때에는 값이 제대로 안 넘어가는 경우가 있다.

	if (false == m_isEnable)
		return E_FAIL;


	switch (m_tInfo.eMonsterType)
	{
	case CUI_MonsterHpFrame::SMALL:
	case CUI_MonsterHpFrame::MID:
	case CUI_MonsterHpFrame::LARGE:
	case CUI_MonsterHpFrame::SIDE:
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
	case CUI_MonsterHpFrame::BOSS:
	case CUI_MonsterHpFrame::NONE:
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

HRESULT CUI_MonsterHpFrame::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	switch (m_tInfo.eMonsterType)
	{
	case CUI_MonsterHpFrame::SMALL:
	{
		//! For.Com_Texture
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EnemyHpFrameSmall"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	}
	case CUI_MonsterHpFrame::MID:
	{
		//! For.Com_Texture
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EnemyHpFrameMid"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	}
	case CUI_MonsterHpFrame::LARGE:
	{
		//! For.Com_Texture
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EnemyHpFrameLarge"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	}
	case CUI_MonsterHpFrame::BOSS:
		break;
	case CUI_MonsterHpFrame::SIDE:
	{
		//! For.Com_Texture
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SideEnemyHpFrameSide"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	}
	case CUI_MonsterHpFrame::NONE:
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CUI_MonsterHpFrame::Bind_ShaderResources()
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

void CUI_MonsterHpFrame::Compute_OwnerCamDistance()
{
	_vector		vPosition = m_tInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	m_fOwnerCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

_bool CUI_MonsterHpFrame::In_Frustum()
{
	return m_pGameInstance->isIn_WorldPlanes(m_tInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION), 2.f);
}

CUI_MonsterHpFrame* CUI_MonsterHpFrame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MonsterHpFrame* pInstance = new CUI_MonsterHpFrame(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_MonsterHpFrame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_MonsterHpFrame::Clone(void* pArg)
{
	CUI_MonsterHpFrame* pInstance = new CUI_MonsterHpFrame(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_MonsterHpFrame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MonsterHpFrame::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

}
