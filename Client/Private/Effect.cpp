#include "stdafx.h"
#include "..\Public\Effect.h"

#include "GameInstance.h"

#include "Easing_Utillity.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CAlphaObject(pDevice, pContext, strPrototypeTag)
{

}

CEffect::CEffect(const CEffect & rhs)
	: CAlphaObject(rhs)
{
}

HRESULT CEffect::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	return S_OK;
}

void CEffect::Priority_Tick(_float fTimeDelta)
{

	
}

void CEffect::Tick(_float fTimeDelta)
{
	//if (m_fWaitingAcc < m_tEffect.fWaitingTime)
	//{
	//	m_fWaitingAcc += fTimeDelta;
	//	//if (m_fWaitingAcc >= m_tEffect.fWaitingTime)
	//	//	m_bRender = true;
	//	//else
	//	//	return;
	//}

	//if (m_fTimeAcc >= m_tEffect.fLifeTime + m_tEffect.fRemainTime)
	//{
	//	m_fTimeAcc = 0.f;

	//	//EffectEnd();
	//	return;
	//}

	//m_fTimeAcc += fTimeDelta;
	//m_fLifeTimeRatio = min(1.0f, m_fTimeAcc / m_tEffect.fLifeTime);

}

void CEffect::Late_Tick(_float fTimeDelta)
{

}

HRESULT CEffect::Render()
{
	return S_OK;
}

HRESULT CEffect::Bind_ShaderResources()
{
	//m_tVariables.vUV_Offset.x = m_tEffect.vUV_Speed.x * m_fTimeAcc;
	//m_tVariables.vUV_Offset.y = m_tEffect.vUV_Speed.y * m_fTimeAcc;

	//if (m_tVariables.vUV_Offset.x > 1.f) m_tVariables.vUV_Offset.x -= 1.f;
	//if (m_tVariables.vUV_Offset.y > 1.f) m_tVariables.vUV_Offset.y -= 1.f;

	//if (m_tEffect.bColor_Lerp)
	//	m_tVariables.vColor_Offset = Easing::LerpToType(m_tEffect.vColor_Start, m_tEffect.vColor_End, m_fTimeAcc, m_tEffect.fLifeTime, EASING_TYPE::LINEAR);
	//else
	//	m_tVariables.vColor_Offset = m_tEffect.vColor_Start;



	return S_OK;
}

CEffect* CEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CEffect* pInstance = new CEffect(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CEffect::Clone(void* pArg)
{
	CEffect* pInstance = new CEffect(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect::Pool()
{
	return new CEffect(*this);
}

void CEffect::Free()
{
	__super::Free();

}

