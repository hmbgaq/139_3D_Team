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
		//m_fEasingTimeAcc = Easing::LerpToType(0.f, m_tEffectDesc.fSequenceTime, m_tEffectDesc.fSequenceAcc, m_tEffectDesc.fSequenceTime, m_tEffectDesc.eType_Easing);

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
			
			if (m_tEffectDesc.fTimeAcc >= m_tEffectDesc.fLifeTime)
			{
				// 삭제 대기시간 누적 시작
				m_tEffectDesc.fRemainAcc += fTimeDelta;

				if (m_tEffectDesc.fRemainAcc >= m_tEffectDesc.fRemainTime)
				{
					End_Effect();
					return;
				}
			}


			/* 파트 이펙트들 Tick */
			for (auto& Pair : m_PartObjects)
			{
				if (nullptr != Pair.second)
				{
					//dynamic_cast<CEffect_Void*>(Pair.second)->Set_TimeAcc(m_fEasingTimeAcc);
					Pair.second->Tick(fTimeDelta);
				}
					
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

_bool CEffect::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);

	Out_Json["Effect"]["bActive_Tool"] = m_tEffectDesc.bActive_Tool;
	Out_Json["Effect"]["bPlay"] = m_tEffectDesc.bPlay;
	Out_Json["Effect"]["bLoop"] = m_tEffectDesc.bLoop;
	Out_Json["Effect"]["bFinished"] = m_tEffectDesc.bFinished;
	Out_Json["Effect"]["bReverse"] = m_tEffectDesc.bReverse;
	Out_Json["Effect"]["bRender"] = m_tEffectDesc.bRender;

	Out_Json["Effect"]["iPartSize"] = m_tEffectDesc.iPartSize;

	Out_Json["Effect"]["eType_Easing"] = m_tEffectDesc.eType_Easing;

	Out_Json["Effect"]["fTimeAcc"] = m_tEffectDesc.fTimeAcc;
	Out_Json["Effect"]["fWaitingAcc"] = m_tEffectDesc.fWaitingAcc;
	Out_Json["Effect"]["fRemainAcc"] = m_tEffectDesc.fRemainAcc;
	Out_Json["Effect"]["fSequenceAcc"] = m_tEffectDesc.fSequenceAcc;

	Out_Json["Effect"]["fLifeTime"] = m_tEffectDesc.fLifeTime;
	Out_Json["Effect"]["fWaitingTime"] = m_tEffectDesc.fWaitingTime;
	Out_Json["Effect"]["fRemainTime"] = m_tEffectDesc.fRemainTime;
	Out_Json["Effect"]["fSequenceTime"] = m_tEffectDesc.fSequenceTime;

	Out_Json["Effect"]["fLifeTimeRatio"] = m_tEffectDesc.fLifeTimeRatio;


	Out_Json["Effect"]["strOwnerTag"] = m_pGameInstance->Convert_WString_To_String(m_tEffectDesc.strOwnerTag);
	
	for (_int i = 0; i < 4; ++i)
		CJson_Utility::Write_Float4(Out_Json["Effect"]["matPivot"][i], XMLoadFloat4x4(&m_tEffectDesc.matPivot).r[i]);

	for (_int i = 0; i < 4; ++i)
		CJson_Utility::Write_Float4(Out_Json["Effect"]["matOffset"][i], XMLoadFloat4x4(&m_tEffectDesc.matOffset).r[i]);


	_int iCount = 0;
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
		{
			Pair.second->Write_Json(Out_Json["Part"][iCount]);
			iCount += 1;
		}
	}

	return true;
}

void CEffect::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

	m_tEffectDesc.bActive_Tool = In_Json["Effect"]["bActive_Tool"];
	m_tEffectDesc.bPlay = In_Json["Effect"]["bPlay"];
	m_tEffectDesc.bLoop = In_Json["Effect"]["bLoop"];
	m_tEffectDesc.bFinished = In_Json["Effect"]["bFinished"];
	m_tEffectDesc.bReverse = In_Json["Effect"]["bReverse"];
	m_tEffectDesc.bRender = In_Json["Effect"]["bRender"];

	m_tEffectDesc.iPartSize = In_Json["Effect"]["iPartSize"];

	m_tEffectDesc.eType_Easing = In_Json["Effect"]["eType_Easing"];

	m_tEffectDesc.fTimeAcc = In_Json["Effect"]["fTimeAcc"];
	m_tEffectDesc.fWaitingAcc = In_Json["Effect"]["fWaitingAcc"];
	m_tEffectDesc.fRemainAcc = In_Json["Effect"]["fRemainAcc"];
	m_tEffectDesc.fSequenceAcc = In_Json["Effect"]["fSequenceAcc"];

	m_tEffectDesc.fLifeTime = In_Json["Effect"]["fLifeTime"];
	m_tEffectDesc.fWaitingTime = In_Json["Effect"]["fWaitingTime"];
	m_tEffectDesc.fRemainTime = In_Json["Effect"]["fRemainTime"];
	m_tEffectDesc.fSequenceTime = In_Json["Effect"]["fSequenceTime"];

	m_tEffectDesc.fLifeTimeRatio = In_Json["Effect"]["fLifeTimeRatio"];

	m_pGameInstance->Convert_WString_To_String(m_tEffectDesc.strOwnerTag) = In_Json["Effect"]["strOwnerTag"];

	_float4x4 matPivot;
	ZeroMemory(&matPivot, sizeof(_float4x4));
	CJson_Utility::Load_JsonFloat4x4(In_Json["Effect"]["matPivot"], matPivot);
	m_tEffectDesc.matPivot = matPivot;


	_float4x4 matOffset;
	ZeroMemory(&matOffset, sizeof(_float4x4));
	CJson_Utility::Load_JsonFloat4x4(In_Json["Effect"]["matOffset"], matOffset);
	m_tEffectDesc.matOffset = matOffset;


	if (m_PartObjects.empty() && 0 < m_tEffectDesc.iPartSize)
	{
		for (_int i = 0; i < m_tEffectDesc.iPartSize; ++i)
		{
			CEffect_Void::EFFECTVOID_DESC	tVoidDesc = {};
			string strTag = "";

			strTag = In_Json["Part"][i]["strProtoTag"];
			m_pGameInstance->String_To_WString(strTag, tVoidDesc.strProtoTag);

			strTag = In_Json["Part"][i]["strPartTag"];
			m_pGameInstance->String_To_WString(strTag, tVoidDesc.strPartTag);
		

			strTag = In_Json["Part"][i]["strModelTag"];
			m_pGameInstance->String_To_WString(strTag, tVoidDesc.strModelTag);
			for (_int j = 0; j < (_int)CEffect_Void::TEXTURE_END; ++j)
			{
				strTag = In_Json["Part"][i]["strTextureTag"][j];
				m_pGameInstance->String_To_WString(strTag, tVoidDesc.strTextureTag[j]);
			}

			tVoidDesc.bUseSpriteAnim = In_Json["Part"][i]["bUseSpriteAnim"];
			tVoidDesc.bUseRigidBody = In_Json["Part"][i]["bUseRigidBody"];

			Ready_PartObjects(tVoidDesc.strProtoTag, tVoidDesc.strPartTag, &tVoidDesc);
		}
	}

	_int iCount = 0;
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
		{
			Pair.second->Load_FromJson(In_Json["Part"][iCount]);
			dynamic_cast<CEffect_Void*>(Pair.second)->Set_Owner(this);
			iCount += 1;
		}
	}

	ReSet_Effect();
}

void CEffect::ReSet_Effect()
{
	m_tEffectDesc.fSequenceAcc	 = 0.f;
	m_tEffectDesc.fTimeAcc		 = 0.f;
	m_tEffectDesc.fWaitingAcc	 = 0.f;
	m_tEffectDesc.fRemainAcc	 = 0.f;
	m_tEffectDesc.fLifeTimeRatio = 0.f;

	m_tEffectDesc.bFinished = FALSE;
	//m_tEffectDesc.bRender	= FALSE;

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
			{
				dynamic_cast<CEffect_Void*>(Pair.second)->End_Effect();
			}				
		}
		ReSet_Effect();
	}
	else
	{
		Set_Dead(TRUE);
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
	//if (nullptr != Find_PartObject(strPrototypeTag))
	//	return E_FAIL;

	CGameObject* pPartObject = m_pGameInstance->Clone_Prototype(strPrototypeTag, pArg);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, pPartObject);
	m_tEffectDesc.iPartSize += 1;

	return S_OK;
}

void CEffect::Set_Owner(CGameObject* pOwner)
{
	m_tEffectDesc.pOwner = pOwner;

}

HRESULT CEffect::Ready_Components()
{


	return S_OK;
}

HRESULT CEffect::Ready_PartObjects(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg)
{
	/* Json로드해서 저장된 파트 오브젝트 준비하도록하자. */

	CGameObject* pPartObject = m_pGameInstance->Clone_Prototype(strPrototypeTag, pArg);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, pPartObject);

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

