#include "stdafx.h"
#include "Body_Infected_A.h"
#include "GameInstance.h"

CBody_Infected_A::CBody_Infected_A(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody_Infected(pDevice, pContext, strPrototypeTag)
{
}

CBody_Infected_A::CBody_Infected_A(const CBody_Infected_A& rhs)
	: CBody_Infected(rhs)
{
}

HRESULT CBody_Infected_A::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBody_Infected_A::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(OptionSetting());

	m_eRender_State = CBody_Infected::RENDER_STATE::ORIGIN;

	return S_OK;
}

void CBody_Infected_A::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Infected_A::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_Infected_A::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Infected_A::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		auto iter = m_vDiscardMesh.find(m_eRender_State);
		if (iter != m_vDiscardMesh.end())  
		{
			auto& Discard = iter->second;
			if (find(Discard.begin(), Discard.end(), i) != Discard.end())
				continue; // 메시번호가 벡터안에있으면 렌더안함 
			else
			{	
				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

				m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
				m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
				m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);

			
				m_pShaderCom->Begin(ECast(MONSTER_SHADER::COMMON_ORIGIN));

				m_pModelCom->Render((_uint)i);
			}
		}
	}

	return S_OK;
}

HRESULT CBody_Infected_A::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}


void CBody_Infected_A::Update_DiscardMesh()
{

}

HRESULT CBody_Infected_A::OptionSetting()
{
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::ORIGIN] = { 2, 5, 6, 7, 8 }; // 피떡 
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::ATTACK] = { 1, 11 }; // 무기 
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::HITTED] = { 2, 5, 6, 7, 8 };
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::NAKED] = {0, 1, 2, 3, 5, 6, 7, 8, 11 }; // 겉가죽 + 의상 + 무기 + 기타 

	return S_OK;
}

HRESULT CBody_Infected_A::Ready_Components()
{	
	FAILED_CHECK(__super::Ready_Components());

	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Infected_A"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	return S_OK;
}

HRESULT CBody_Infected_A::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));

	return S_OK;
}

CBody_Infected_A* CBody_Infected_A::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Infected_A* pInstance = new CBody_Infected_A(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Infected_A");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Infected_A::Clone(void* pArg)
{
	CBody_Infected_A* pInstance = new CBody_Infected_A(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Infected_A");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Infected_A::Pool()
{
	return new CBody_Infected_A(*this);
}

void CBody_Infected_A::Free()
{
	__super::Free();
}
