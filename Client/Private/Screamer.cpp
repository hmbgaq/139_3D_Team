#include "Screamer.h"
#include "GameInstance.h"
#include "Model.h"

CScreamer::CScreamer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CScreamer::CScreamer(const CScreamer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CScreamer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CScreamer::Initialize(void* pArg)
{
	FAILED_CHECK(Ready_Components());

	m_iRenderPass = 0;

	m_pTransformCom->Set_Scaling(0.01f, 0.01f, 0.01f);
	m_pTransformCom->Set_Position(_float3(25.f, 0.5f, 10.f));
	m_vBloomColor = { 0.5f, 0.f, 0.5f, 1.f };
	m_pModelCom->Set_Animation(3, CModel::ANIM_STATE::ANIM_STATE_STOP, true);

	return S_OK;
}

void CScreamer::Priority_Tick(_float fTimeDelta)
{
}

void CScreamer::Tick(_float fTimeDelta)
{
	/* Example - Animation*/
	//if (m_pGameInstance->Key_Down(DIK_3))
	//{
	//	m_pModelCom->Set_Animation(3, CModel::ANIM_STATE::ANIM_STATE_NORMAL, true);
	//}
	//if (m_pGameInstance->Key_Down(DIK_4))
	//{
	//	m_pModelCom->Set_Animation(3, CModel::ANIM_STATE::ANIM_STATE_LOOP, true);
	//}
	//if (m_pGameInstance->Key_Down(DIK_5))
	//{
	//	m_pModelCom->Set_Animation(3, CModel::ANIM_STATE::ANIM_STATE_STOP, true);
	//}
	//if (m_pGameInstance->Key_Down(DIK_6))
	//{
	//	m_pModelCom->Set_Animation(3, CModel::ANIM_STATE::ANIM_STATE_REVERSE, false); /* 문제있음 쓰지마셈 */
	//}

	if (m_pGameInstance->Key_Down(DIK_9))
	{
		m_iRenderPass += 1;
		if (m_iRenderPass >= ECast(ANIM_SHADER::ANIM_SHADER_END))
			m_iRenderPass = 0;

		cout << "Render Pass : " << m_iRenderPass << endl;
	}

	if (m_pGameInstance->Key_Down(DIK_8))
	{
		m_vBloomColor += _float4(fTimeDelta, 0.f, 0.f, 0.f);
	}

	m_fTimeDelta += fTimeDelta;
	m_fDissolveWeight += fTimeDelta * 0.5f;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CScreamer::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Transform_Frustum_ToLocalSpace(m_pTransformCom->Get_WorldMatrix());

	if (true == m_pGameInstance->isIn_LocalPlanes(XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_WorldMatrixInverse()), 10.f))
	{
		_float3 vPos;
		m_pModelCom->Play_Animation(fTimeDelta, vPos);

		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this), );
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this), );

	}


	m_pGameInstance->Add_DebugRender(m_pColliderCom);
}

HRESULT CScreamer::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_MetallicTexture", (_uint)i, aiTextureType_METALNESS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_RougnessTexture", (_uint)i, aiTextureType_DIFFUSE_ROUGHNESS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_OcclusionTexture", (_uint)i, aiTextureType_AMBIENT_OCCLUSION);
		
		m_pShaderCom->Begin(m_iRenderPass);

		m_pModelCom->Render(_uint(i));
	}

	return S_OK;
}

HRESULT CScreamer::Render_Shadow()
{
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4()));

	_float4x4		ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(-20.f, 20.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / (float)g_iWinSizeY, 0.1f, m_pGameInstance->Get_CamFar()));

	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix));

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(2);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CScreamer::Ready_Components()
{
	/* For. Transform */
	{
		CGameObject::GAMEOBJECT_DESC tTransformDESC = {};
		tTransformDESC.fRotationPerSec = 10.f;
		tTransformDESC.fSpeedPerSec = 10.f;

		FAILED_CHECK(__super::Initialize(&tTransformDESC));
	}

	/* For.Com_Shader */
	{
		FAILED_CHECK(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"),	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}

	/* For.Com_Model */
	{

		FAILED_CHECK(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Screamer"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));
	
	}

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	{
		BoundingDesc.fRadius = 200.f;
		BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);
		
		FAILED_CHECK(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc));
	}

	/* For. Com_Texture*/
	{
		FAILED_CHECK(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BreakMask"), TEXT("Com_BreakMask"), reinterpret_cast<CComponent**>(&m_pBreakTextureCom)));
		FAILED_CHECK(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTex"), reinterpret_cast<CComponent**>(&m_pDissolveTexCom)));
	}

	return S_OK;

}

HRESULT CScreamer::Bind_ShaderResources()
{
	/* Base */
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	/* Variable */
	m_pShaderCom->Bind_RawValue("g_TimeDelta", &m_fTimeDelta, sizeof(_float));
	m_pShaderCom->Bind_RawValue("g_fDissolveWeight", &m_fDissolveWeight, sizeof(_float));
	m_pShaderCom->Bind_RawValue("g_BloomColor", &m_vBloomColor, sizeof(_float4));

	/* Texture */
	m_pBreakTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskingTexture");
	m_pDissolveTexCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture");

	return S_OK;
}

CScreamer* CScreamer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CScreamer* pInstance = new CScreamer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CScreamer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CScreamer::Clone(void* pArg)
{
	CScreamer* pInstance = new CScreamer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CScreamer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CScreamer::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
