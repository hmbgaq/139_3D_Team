#include "..\Public\Crane.h"
#include "Model.h"
#include "Shader.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Character.h"

CCrane::CCrane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CCrane::CCrane(const CCrane& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCrane::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CCrane::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	Ready_Components();

	return S_OK;
}

void CCrane::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CCrane::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//m_pColliderCom->Update(m_pTransformCom->Get_WorldFloat4x4());

	if (m_pGameInstance->Key_Down(DIK_Y))
	{
		m_pModelCom->Set_Animation(0, CModel::ANIM_STATE::ANIM_STATE_NORMAL, true, false, 0);
	}
}

void CCrane::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (true)// == m_pGameInstance->isIn_WorldPlanes(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pModelCom->Play_Animation(fTimeDelta, _float3(0.f, 0.f, 0.f));

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
			return;

//#ifdef _DEBUG
//		m_pGameInstance->Add_DebugRender(m_pColliderCom);
//#endif	
	}

}

HRESULT CCrane::Render()
{
	FAILED_CHECK(__super::Render());

	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CCrane::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

void CCrane::OnCollisionEnter(CCollider* other)
{
}

void CCrane::OnCollisionStay(CCollider* other)
{
}

void CCrane::OnCollisionExit(CCollider* other)
{
}

HRESULT CCrane::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrnetLevel, L"Prototype_Component_Model_Crane",
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrane::Bind_ShaderResources()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	return S_OK;
}


CCrane* CCrane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CCrane* pInstance = new CCrane(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCrane");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCrane::Clone(void* pArg)
{
	CCrane* pInstance = new CCrane(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCrane");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCrane::Pool()
{
	return new CCrane(*this);
}

void CCrane::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	//Safe_Release(m_pColliderCom);
}
