#include "Screamer.h"
#include "GameInstance.h"
#include "Model.h"
//#include "UI.h"
//#include "UI_Weakness.h"

CScreamer::CScreamer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
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

	m_iRenderPass = ECast(ANIM_SHADER::ANIM_ORIGIN);

	
	m_pTransformCom->Set_Position(_float3(15.f, 0.f, 10.f));
	m_vBloomColor = { 0.5f, 0.5f, 0.5f, 1.f };
	m_pModelCom->Set_Animation(3, CModel::ANIM_STATE::ANIM_STATE_STOP, true);

	///* Test UI */
	m_pWeakneesUI = dynamic_cast<CUI_Weakness*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Weakness")));
	//m_pWeakneesUI->SetUp_PositionToScreen(m_pTransformCom->Get_Position());
	m_pWeakneesUI->SetUp_WorldToScreen(m_pTransformCom->Get_WorldMatrix());

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

	if (m_pGameInstance->Key_Down(DIK_G))
		m_bTestUI = !m_bTestUI;

	if (m_bTestUI)
	{
		//m_pWeakneesUI->SetUp_PositionToScreen(m_pTransformCom->Get_Position());
		m_pWeakneesUI->SetUp_WorldToScreen(m_pTransformCom->Get_WorldMatrix());
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
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_OUTLINE, this), );

		m_pGameInstance->Add_DebugRender(m_pColliderCom);
	}

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
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);
		
		m_pShaderCom->Begin(m_iRenderPass);

		m_pModelCom->Render(_uint(i));
	}

	return S_OK;
}

HRESULT CScreamer::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_pGameInstance->Get_NextLevel());
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4()));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_pGameInstance->Get_NextLevel())));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_pGameInstance->Get_NextLevel())));

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(2);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CScreamer::Render_OutLine()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_float m_fLineThick = 0.5f;
		m_pShaderCom->Bind_RawValue("g_LineThick", &m_fLineThick, sizeof(_float));

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);

		m_pShaderCom->Begin(ECast(ANIM_SHADER::ANIM_OUTLINE));

		m_pModelCom->Render(0);
	}

	return S_OK;
}

HRESULT CScreamer::Render_Cascade_Shadow(_uint i)
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return S_OK;
	
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	
	FAILED_CHECK(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i));
	
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_CascadeProj", &m_pGameInstance->Get_Shadow_Proj()));
	
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		FAILED_CHECK(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture"));
	
		FAILED_CHECK(m_pModelCom->Render(m_pShaderCom, i,ECast(ANIM_SHADER::ANIM_CASCADE_SHADOW)));
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

	_int iCurrentLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	{
		FAILED_CHECK(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Shader_AnimModel"),	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}

	/* For.Com_Model */
	{

		FAILED_CHECK(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Model_Screamer"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));
	
	}

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	{
		BoundingDesc.fRadius = 1.5f;
		BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);
		
		FAILED_CHECK(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc));
	}

	/* For. Com_Texture*/
	{
		//FAILED_CHECK(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Texture_BreakMask"), TEXT("Com_BreakMask"), reinterpret_cast<CComponent**>(&m_pBreakTextureCom)));
		//FAILED_CHECK(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTex"), reinterpret_cast<CComponent**>(&m_pDissolveTexCom)));
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
	_float gCamFar = m_pGameInstance->Get_CamFar();
	m_pShaderCom->Bind_RawValue("g_fCamFar", &gCamFar, sizeof(_float));
	m_pShaderCom->Bind_RawValue("g_TimeDelta", &m_fTimeDelta, sizeof(_float));
	m_pShaderCom->Bind_RawValue("g_fDissolveWeight", &m_fDissolveWeight, sizeof(_float));
	m_pShaderCom->Bind_RawValue("g_BloomColor", &m_vBloomColor, sizeof(_float4));

	/* Texture */
	m_pBreakTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskingTexture");
	m_pDissolveTexCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture");

	m_vRimColor = { 0.0f, 1.f, 0.f, 1.f };
	m_vRimPower = _float3(1.0f, 1.0f, 1.0f);


	/* RimLight */
	m_vCamPos = m_pGameInstance->Get_CamPosition();
	m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4));
	m_pShaderCom->Bind_RawValue("g_vRimPower", &m_vRimPower, sizeof(_float3));
	m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPos, sizeof(_float4));

	return S_OK;
}

CScreamer* CScreamer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CScreamer* pInstance = new CScreamer(pDevice, pContext, strPrototypeTag);

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

CGameObject* CScreamer::Pool()
{
	return new CScreamer(*this);
}

void CScreamer::Free()
{
	__super::Free();

	Safe_Release(m_pDissolveTexCom);
	Safe_Release(m_pBreakTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}


