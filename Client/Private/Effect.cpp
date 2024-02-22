#include "stdafx.h"
#include "..\Public\Effect.h"

#include "GameInstance.h"

#include "Effect_Particle.h"
#include "Effect_Rect.h"
#include "Effect_Instance.h"
#include "Effect_Trail.h"


CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{

}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{	
	m_tEffectDesc = *(EFFECT_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	return S_OK;
}

void CEffect::Priority_Tick(_float fTimeDelta)
{
	if (m_tEffectDesc.bActive_Tool)
	{
		for (auto& Pair : m_PartObjects)
		{
			if (nullptr != Pair.second)
				Pair.second->Priority_Tick(fTimeDelta);
		}
	}

}

void CEffect::Tick(_float fTimeDelta)
{
	if (m_tEffectDesc.bActive_Tool)
	{
		m_tEffectDesc.fSequenceTime = m_tEffectDesc.fWaitingTime + m_tEffectDesc.fLifeTime + m_tEffectDesc.fRemainTime;

		if (m_tEffectDesc.bPlay)
		{
			m_tEffectDesc.fSequenceAcc += fTimeDelta;

			// 시작지연 누적시간이 지나면 렌더 시작(이펙트 시작)
			if (m_tEffectDesc.fWaitingAcc < m_tEffectDesc.fWaitingTime)
			{
				m_tEffectDesc.fWaitingAcc += fTimeDelta;

				if (m_tEffectDesc.fWaitingAcc >= m_tEffectDesc.fWaitingTime)
					m_tEffectDesc.bRender = true;
				else
					return;
			}

			// 시간 누적 시작
			m_tEffectDesc.fTimeAcc += fTimeDelta;
			m_tEffectDesc.fLifeTimeRatio = min(1.0f, m_tEffectDesc.fTimeAcc / m_tEffectDesc.fLifeTime);
			if (m_tEffectDesc.fTimeAcc >= m_tEffectDesc.fLifeTime + m_tEffectDesc.fRemainTime)
			{
				End_Effect();
				return;
			}


			/* 파트 이펙트들 Tick */
			for (auto& Pair : m_PartObjects)
			{
				if (nullptr != Pair.second)
					Pair.second->Tick(fTimeDelta);
			}

		}
	}
}

void CEffect::Late_Tick(_float fTimeDelta)
{
	if (m_tEffectDesc.bActive_Tool)
	{
		for (auto& Pair : m_PartObjects)
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Tick(fTimeDelta);
		}
	}
}

HRESULT CEffect::Render()
{
	if (m_tEffectDesc.bActive_Tool)
	{
		if (m_tEffectDesc.bRender)
		{
			for (auto& Pair : m_PartObjects)
			{
				if (nullptr != Pair.second)
					Pair.second->Render();
			}
		}
	}

	return S_OK;
}

void CEffect::ReSet_Effect()
{
	m_tEffectDesc.fSequenceAcc	 = 0.f;
	m_tEffectDesc.fTimeAcc		 = 0.f;
	m_tEffectDesc.fWaitingAcc	 = 0.f;
	m_tEffectDesc.fLifeTimeRatio = 0.f;

	m_tEffectDesc.bFinished = FALSE;
	m_tEffectDesc.bRender	= FALSE;

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			dynamic_cast<CEffect_Void*>(Pair.second)->ReSet_Effect();
	}
}

void CEffect::End_Effect()
{
	m_tEffectDesc.bFinished = TRUE;

	if (m_tEffectDesc.bLoop)
	{	
		for (auto& Pair : m_PartObjects)
		{
			if (nullptr != Pair.second)
				dynamic_cast<CEffect_Void*>(Pair.second)->End_Effect();
		}

		ReSet_Effect();
	}

}


CGameObject* CEffect::Find_PartObject(const wstring& strPartTag)
{
	auto	iter = m_PartObjects.find(strPartTag);

	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second;
}

HRESULT CEffect::Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg)
{
	if (nullptr != Find_PartObject(strPrototypeTag))
		return E_FAIL;

	CGameObject* pPartObject = m_pGameInstance->Clone_Prototype(strPrototypeTag, pArg);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, pPartObject);

	return S_OK;
}

HRESULT CEffect::Ready_Components()
{


	return S_OK;
}

HRESULT CEffect::Ready_PartObjects()
{
	/* Json로드해서 저장된 파트 오브젝트 준비하도록하자. */



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

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);

	m_PartObjects.clear();

}

