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


	m_strOwnerModelTag = ((PART_PREVIEW_DESC*)pArg)->strOwnerModelTag;

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

	if (nullptr != m_pTrail)
	{
		m_pTrail->Tick_Trail(fTimeDelta, m_WorldMatrix);
	}

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


	if (m_strOwnerModelTag == TEXT("Prototype_Component_Model_Heavy_Vampiric_Zombie"))
	{
		/* For.Render */
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;
	}
}

HRESULT CPart_Preview::Render()
{
	__super::Render();

	if (m_strOwnerModelTag == TEXT("Prototype_Component_Model_Heavy_Vampiric_Zombie"))
	{
		FAILED_CHECK(Bind_ShaderResources());


		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
			m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
			m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);

			m_pShaderCom->Begin(0);

			m_pModelCom->Render((_uint)i);
		}
	}

	return S_OK;
}

HRESULT CPart_Preview::Ready_Trail(_uint iLevelIndex, string strFileName)
{
	m_pTrail = EFFECT_MANAGER->Ready_Trail(iLevelIndex, LAYER_EFFECT, strFileName, this);

	return S_OK;
}


HRESULT CPart_Preview::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();


	if (m_strOwnerModelTag == TEXT("Prototype_Component_Model_Heavy_Vampiric_Zombie"))
	{
		_matrix matOffset =
		{
			0.6672178506851196f,-0.46065229177474976f, -0.5852329134941101f, 0.0f,
			0.6070240139961243f, -0.11897064000368118f, 0.785706639289856f, 0.f,
			-0.4315733313560486f, -0.8795087337493896f, 0.20025238394737244f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		m_pTransformCom->Set_WorldMatrix(matOffset);

		/* For.Com_Model */
		FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Heavy_Vampiric_Zombie_Weapon"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

		/* For. Com_Shader */
		FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));

	}


	return S_OK;
}

HRESULT CPart_Preview::Bind_ShaderResources()
{
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

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

	if (nullptr != m_pTrail)
	{
		m_pTrail->Set_Object_Owner(nullptr);
		Safe_Release(m_pTrail);
	}

	if (nullptr != m_pShaderCom)
	{
		Safe_Release(m_pShaderCom);
	}

	if (nullptr != m_pShaderCom)
	{
		Safe_Release(m_pModelCom);
	}
		
}