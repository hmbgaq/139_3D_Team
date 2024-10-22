#include "stdafx.h"
#include "Interact_Chain.h"
#include "Model.h"
#include "Shader.h"
#include "GameInstance.h"

CInteract_Chain::CInteract_Chain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CInteract_Chain::CInteract_Chain(const CInteract_Chain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CInteract_Chain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteract_Chain::Initialize(void* pArg)
{
	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_fLineThick = 1.2f;

	m_pTransformCom->Set_Position(_float3(25.f, 0.f, 7.f));
	return S_OK;
}

void CInteract_Chain::Priority_Tick(_float fTimeDelta)
{
}

void CInteract_Chain::Tick(_float fTimeDelta)
{
	if (m_bInteractActive)
	{
		if (m_bIncrease)
		{
			m_fTimeAcc += fTimeDelta;

			if (m_fTimeAcc > 1)
				m_bIncrease = false;
		}
		else
		{
			m_fTimeAcc -= fTimeDelta;
			if (m_fTimeAcc < 0 )
				m_bIncrease = true;
		}

		m_vLineColor = { 1.f, 1.f, 1.f, m_fTimeAcc };
	}
	else
		m_fTimeAcc = 0.f;
}

void CInteract_Chain::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Transform_Frustum_ToLocalSpace(m_pTransformCom->Get_WorldMatrix());

	if (true == m_pGameInstance->isIn_LocalPlanes(XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_WorldMatrixInverse()), 0.f))
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this), );
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_OUTLINE, this), );
	}

}

HRESULT CInteract_Chain::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE); // 1
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS); // 6
		/* 체인부분만 하얗게 변하고 나무부분은 원래 Diffuse로 그리도록함 */

		if (iNumMeshes >= 2)
		{
			if (0 == i) /* 체인 */
			{
				if (m_bInteractActive)
					iRenderPass = ECast(MODEL_SHADER::MODEL_WHITEBLINK); // 3
				else
					iRenderPass = ECast(MODEL_SHADER::MODEL_ORIGIN);

				m_pShaderCom->Begin(iRenderPass);
				m_pModelCom->Render((_uint)i);
			}
			else if (0 != i) /* 나무 */
			{
				m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_ORIGIN));

				m_pModelCom->Render((_uint)i);
			}
		}
		else
		{
		m_pShaderCom->Begin(iRenderPass);

		m_pModelCom->Render((_uint)i);
		}
	}

	return S_OK;
}

HRESULT CInteract_Chain::Render_OutLine()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	m_pShaderCom->Bind_RawValue("g_vLineColor", &m_vLineColor, sizeof(_float4));
	m_pShaderCom->Bind_RawValue("g_LineThick", &m_fLineThick, sizeof(_float));
	m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fTimeAcc, sizeof(_float));

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_OUTLINE_BLINK));
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CInteract_Chain::Ready_Components()
{
	/* For. Transform*/
	{
		FAILED_CHECK(__super::Initialize(nullptr));
	}

	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	{
		FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}

	/* For.Com_Model */
	{
		FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_ChainBeam4"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));
	}

	return S_OK;
}

HRESULT CInteract_Chain::Bind_ShaderResources()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	if (m_bInteractActive)
	{
		m_pShaderCom->Bind_RawValue("g_vLineColor", &m_vLineColor, sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_LineThick", &m_fLineThick, sizeof(_float));
		m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fTimeAcc, sizeof(_float));
	}

	return S_OK;
}

CInteract_Chain* CInteract_Chain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CInteract_Chain* pInstance = new CInteract_Chain(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInteract_Chain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInteract_Chain::Clone(void* pArg)
{
	CInteract_Chain* pInstance = new CInteract_Chain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteract_Chain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInteract_Chain::Pool()
{
	return new CInteract_Chain(*this);
}

void CInteract_Chain::Free()
{
	__super::Free();

	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}


