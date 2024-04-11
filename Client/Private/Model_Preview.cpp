#include "stdafx.h"
#include "Model_Preview.h"

#include "GameInstance.h"

#include "Part_Preview.h"

CModel_Preview::CModel_Preview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
	m_bIsPoolObject = FALSE;
}

CModel_Preview::CModel_Preview(const CModel_Preview& rhs)
	: CGameObject(rhs)
{
}

HRESULT CModel_Preview::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Preview::Initialize(void* pArg)
{
	MODEL_PREVIEW_DESC* pDesc = (MODEL_PREVIEW_DESC*)pArg;


	m_tDesc.strProtoTag = pDesc->strProtoTag;
	m_tDesc.strModelTag = pDesc->strModelTag;

	m_tDesc.iAnimIndex = pDesc->iAnimIndex;
	m_tDesc.iRenderGroup = pDesc->iRenderGroup;
	m_tDesc.iShaderPassIndex = pDesc->iShaderPassIndex;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	if (CModel::TYPE_ANIM == m_tDesc.eType)
	{
		if (TEXT("Prototype_Component_Model_Rentier") == m_tDesc.strModelTag)
		{
			if (FAILED(Ready_PartObjects()))
				return E_FAIL;
		}


		m_pModelCom->Set_Animation(m_tDesc.iAnimIndex);
	}


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));


	if (TEXT("Prototype_Component_Model_EffectTool_IntroBoss_Floor") == m_tDesc.strModelTag)
	{
		Set_Position(_float3(-15.f, -0.5f, 0.f));
		m_pTransformCom->Set_Scaling(15.f, 1.f, 15.f);
	}


	return S_OK;
}

void CModel_Preview::Priority_Tick(_float fTimeDelta)
{
	if (m_tDesc.bRender)
	{
		__super::Priority_Tick(fTimeDelta);


		for (auto& Pair : m_PartObjects)
		{
			if (nullptr != Pair.second)
				Pair.second->Priority_Tick(fTimeDelta);
		}

	}

}

void CModel_Preview::Tick(_float fTimeDelta)
{

	if (m_tDesc.bRender)
	{
		__super::Tick(fTimeDelta);

		Move(fTimeDelta);

		for (auto& Pair : m_PartObjects)
		{
			if (nullptr != Pair.second)
				Pair.second->Tick(fTimeDelta);
		}

	}
}

void CModel_Preview::Late_Tick(_float fTimeDelta)
{
	if (m_tDesc.bRender)
	{
		__super::Late_Tick(fTimeDelta);
	
		for (auto& Pair : m_PartObjects)
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Tick(fTimeDelta);
		}

		//if (TRUE == m_pGameInstance->isIn_WorldPlanes(m_pTransformCom->Get_Position(), 2.f))
		{
			if (CModel::TYPE_ANIM == m_tDesc.eType)
			{
				if (m_tDesc.bPlay)
				{
					m_pModelCom->Play_Animation(fTimeDelta, TRUE);
				}
			}
		

			if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP(m_tDesc.iRenderGroup), this)))
				return;
		}
	}
}

HRESULT CModel_Preview::Render()
{
	if (m_tDesc.bRender)
	{
		__super::Render();

		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (CModel::TYPE_ANIM == m_tDesc.eType)
			{
				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
			}

			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
			m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
			m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

			m_pShaderCom->Begin(m_tDesc.iShaderPassIndex);

			m_pModelCom->Render((_uint)i);
		}
	}

	return S_OK;
}

void CModel_Preview::Set_AnimIndex(_uint iAnimIndex)
{
	if (CModel::TYPE_ANIM == m_tDesc.eType)
	{
		m_tDesc.iAnimIndex = iAnimIndex;
		m_pModelCom->Set_Animation(m_tDesc.iAnimIndex);
	}

}

void CModel_Preview::Move(_float fTimeDelta)
{

	// ALT + 방향키
	if (m_pGameInstance->Key_Pressing(DIK_LMENU))
	{
		if (m_pGameInstance->Key_Pressing(DIK_UP))
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 0.5f);

		}
		else if (m_pGameInstance->Key_Pressing(DIK_DOWN))
		{
			m_pTransformCom->Go_Backward(fTimeDelta * 0.5f);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_LEFT))
		{
			m_pTransformCom->Go_Left(fTimeDelta * 0.5f);

		}
		else if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
		{
			m_pTransformCom->Go_Right(fTimeDelta * 0.5f);
		}
	}

}

CGameObject* CModel_Preview::Find_PartObject(const wstring& strPartTag)
{
	auto	iter = m_PartObjects.find(strPartTag);

	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second;
}

HRESULT CModel_Preview::Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg)
{
	//if (nullptr != Find_PartObject(strPrototypeTag))
	//	return E_FAIL;

	CGameObject* pPartObject = m_pGameInstance->Clone_Prototype(strPrototypeTag, pArg);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, pPartObject);

	return S_OK;
}

HRESULT CModel_Preview::Ready_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TOOL,m_tDesc.strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	m_tDesc.eType = m_pModelCom->Get_ModelType();


	/* For.Com_Shader */
	if (CModel::TYPE_ANIM == m_tDesc.eType)
	{
		if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Monster"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Model"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CModel_Preview::Ready_PartObjects()
{
	CPart_Preview::PART_PREVIEW_DESC	tPartDesc = {};

	tPartDesc.fSpeedPerSec		= { 0.f };
	tPartDesc.fRotationPerSec	= { 0.f };
	
	if (TEXT("Prototype_Component_Model_Rentier") == m_tDesc.strModelTag)
	{
		tPartDesc.pSocketBone = m_pModelCom->Get_BonePtr("RightHandIK");	// "LeftHandIK" , "RightHandIK"
	}

	if (TEXT("Prototype_Component_Model_VampireCommander") == m_tDesc.strModelTag)
	{
		tPartDesc.pSocketBone = m_pModelCom->Get_BonePtr("RightHandIK");	// "LeftHandIK" , "RightHandIK"
	}

	tPartDesc.pParentTransform = { m_pTransformCom };
	tPartDesc.pOwner = { this };

	/* For.Part_Preview*/
	if (FAILED(Add_PartObject(TEXT("Prototype_GameObject_Part_Preview"), TEXT("Part_Preview"), &tPartDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Preview::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));

	return S_OK;
}

CModel_Preview* CModel_Preview::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CModel_Preview* pInstance = new CModel_Preview(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CModel_Preview");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CModel_Preview::Clone(void* pArg)
{
	CModel_Preview* pInstance = new CModel_Preview(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel_Preview");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CModel_Preview::Pool()
{
	return new CModel_Preview(*this);
}

void CModel_Preview::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

