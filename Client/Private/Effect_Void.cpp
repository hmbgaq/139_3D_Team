#include "stdafx.h"
#include "..\Public\Effect_Void.h"


CEffect_Void::CEffect_Void(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CAlphaObject(pDevice, pContext, strPrototypeTag)
{

}

CEffect_Void::CEffect_Void(const CEffect_Void & rhs)
	: CAlphaObject(rhs)
{
}

HRESULT CEffect_Void::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_Void::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	return S_OK;
}

void CEffect_Void::Priority_Tick(_float fTimeDelta)
{

	
}

void CEffect_Void::Tick(_float fTimeDelta)
{
	/* 예시 틀 백업 */
	//if (m_tEffectDesc.bActive_Tool)
	//{
	//	m_fSequenceTime = m_fLifeTime + m_fRemainTime;

	//	if (m_tEffectDesc.bPlay)
	//	{
	//		// 시작지연 누적시간이 지나면 렌더 시작(이 이펙트 시작)
	//		if (m_fWaitingAcc <= m_fWaitingTime)
	//		{
	//			m_fWaitingAcc += fTimeDelta;

	//			if (m_fWaitingAcc >= m_fWaitingTime)
	//			{
	//				m_tEffectDesc.bRender = TRUE;
	//			}
	//			else
	//				return;
	//		}

	//		// 라이프타임 누적 시작
	//		m_fTimeAcc += fTimeDelta;
	//		m_fLifeTimeRatio = min(1.0f, m_fTimeAcc / m_fLifeTime);

	//		/* ======================= 라이프 타임 동작 시작 ======================= */




	//		/* ======================= 라이프 타임 동작 끝  ======================= */

	//		if (m_fTimeAcc >= m_fLifeTime)
	//		{
	//			// 삭제 대기시간 누적 시작
	//			m_fRemainAcc += fTimeDelta;

	//			// 디졸브 시작
	//			m_tEffectDesc.bDissolve = TRUE;
	//			if (m_tEffectDesc.bDissolve)
	//			{
	//				m_tEffectDesc.fDissolveAmount = Easing::LerpToType(0.f, 1.f, m_fRemainAcc, m_fRemainTime, m_tEffectDesc.eType_Easing);
	//			}
	//			// 디졸브 끝


	//			// 이 이펙트의 타임라인 종료
	//			if (m_fRemainAcc >= m_fRemainTime)
	//			{
	//				m_tEffectDesc.bRender = TRUE;
	//				return;
	//			}
	//		}
	//	}
	//}

}

void CEffect_Void::Late_Tick(_float fTimeDelta)
{

}

HRESULT CEffect_Void::Render()
{
	return S_OK;
}

void CEffect_Void::ReSet_Effect()
{
	//__super::ReSet_Effect();

	m_fSequenceAcc		= 0.f;
	m_fTimeAcc			= 0.f;
	m_fWaitingAcc		= 0.f;
	m_fLifeTimeRatio	= 0.f;
	m_fRemainAcc		= 0.f;

	//m_tEffectDesc.fDissolveAmount = 0.f;
	//m_tEffectDesc.bDissolve		 = FALSE;
	//m_tEffectDesc.bRender		 = FALSE;

}

void CEffect_Void::End_Effect()
{
	//__super::End_Effect();

	//if (m_tEffectDesc.bLoop)
	//{
	//	ReSet_Effect();
	//}

}



void CEffect_Void::Free()
{
	__super::Free();

}

