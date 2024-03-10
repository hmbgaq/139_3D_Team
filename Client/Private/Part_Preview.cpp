#include "stdafx.h"
#include "..\Public\Part_Preview.h"

#include "GameInstance.h"
#include "Effect_Manager.h"

#include "Bone.h"
#include "Effect_Trail.h"

CPart_Preview::CPart_Preview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
	m_bIsPoolObject = FALSE;
}

CPart_Preview::CPart_Preview(const CPart_Preview & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPart_Preview::Initialize_Prototype()
{	
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Preview::Initialize(void* pArg)
{	
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_pParentTransform = ((PART_PREVIEW_DESC*)pArg)->pParentTransform;
	m_pOwner = ((PART_PREVIEW_DESC*)pArg)->pOwner;
	if (nullptr == m_pParentTransform)
		return E_FAIL;
	Safe_AddRef(m_pParentTransform);


	m_pSocketBone = ((PART_PREVIEW_DESC*)pArg)->pSocketBone;

	if (nullptr == m_pSocketBone)
		return E_FAIL;
	Safe_AddRef(m_pSocketBone);


	FAILED_CHECK(__super::Initialize(pArg));


	FAILED_CHECK(Ready_Components());


	//FAILED_CHECK(Ready_Trail(LEVEL_TOOL));	// 트레일 로드 테스트


	return S_OK;
}

void CPart_Preview::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

}

void CPart_Preview::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


}

void CPart_Preview::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	_matrix		SocketMatrix = m_pSocketBone->Get_CombinedTransformationMatrix();

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * m_pParentTransform->Get_WorldMatrix());


	///* For.Render */
	//if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
	//	return;



	if (nullptr != m_pTrail)
	{
		//m_pTrail->Set_Play(m_bTrailPlay);
		m_pTrail->Tick_Trail(fTimeDelta, m_WorldMatrix);
	}
}

HRESULT CPart_Preview::Render()
{
	__super::Render();


	return S_OK;
}

HRESULT CPart_Preview::Ready_Trail(_uint iLevelIndex, string strFileName)
{
	m_pTrail = EFFECT_MANAGER->Ready_Trail(iLevelIndex, LAYER_EFFECT, strFileName);

	return S_OK;
}


HRESULT CPart_Preview::Ready_Components()
{



	return S_OK;
}

HRESULT CPart_Preview::Bind_ShaderResources()
{


	return S_OK;
}

CPart_Preview * CPart_Preview::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CPart_Preview*		pInstance = new CPart_Preview(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다. */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPart_Preview");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPart_Preview::Clone(void* pArg)
{
	CPart_Preview*		pInstance = new CPart_Preview(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPart_Preview");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CPart_Preview::Pool()
{
	return new CPart_Preview(*this);
}


void CPart_Preview::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);	

	if(nullptr != m_pTrail)
		Safe_Release(m_pTrail);
}