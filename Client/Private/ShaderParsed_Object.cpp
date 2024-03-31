#include "ShaderParsed_Object.h"
#include "GameInstance.h"
#include "Data_Manager.h"
#include "Model.h"
#include "SMath.h"

CShaderParsed_Object::CShaderParsed_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CShaderParsed_Object::CShaderParsed_Object(const CShaderParsed_Object& rhs)
	: CGameObject(rhs)
{
}

HRESULT CShaderParsed_Object::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShaderParsed_Object::Initialize(void* pArg)
{
	FAILED_CHECK(Ready_Components(pArg));

	return S_OK;
}

HRESULT CShaderParsed_Object::Ready_Components(void* pArg)
{
	CREATE_DESC* Desc = (CREATE_DESC*)pArg;

	_int iCurrentLevel = m_pGameInstance->Get_NextLevel();

	/* For. Transform */
	{
		CGameObject::GAMEOBJECT_DESC tTransformDESC = {};
		tTransformDESC.fRotationPerSec = 10.f;
		tTransformDESC.fSpeedPerSec = 10.f;
		FAILED_CHECK(__super::Initialize(&tTransformDESC));
	}

	/* For.Com_Shader */
	{
		FAILED_CHECK(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Shader_Monster"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
		//FAILED_CHECK(__super::Add_Component(iCurrentLevel, SMath::string_To_WString(Desc->strShaderProtoTag), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}

	/* For.Com_Model */
	{
		wstring ModelTag = SMath::string_To_WString(Desc->strModelProtoTag);
		FAILED_CHECK(__super::Add_Component(iCurrentLevel, ModelTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));
	}

	/* For.Com_Collider */
	{
		CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
		BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER);
		BoundingDesc.vExtents = _float3(0.3f, 0.5f, 0.3f);
		BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

		FAILED_CHECK(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc));
	}

	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);
	m_eType = Desc->eType;
	m_iRenderPass = Desc->iRenderPass;

	return S_OK;
}

void CShaderParsed_Object::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CShaderParsed_Object::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CShaderParsed_Object::Late_Tick(_float fTimeDelta)
{
	_float3		m_vMovePos = { 0.f, 0.f, 0.f };

	m_pModelCom->Play_Animation(fTimeDelta, m_vMovePos);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this), );
}

HRESULT CShaderParsed_Object::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_eRenderType == RENDER_TYPE::NORMAL)
	{
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (m_eType == CModel::TYPE::TYPE_ANIM)
				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
			m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
			m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));
			m_pShaderCom->Begin(ECast(MONSTER_SHADER::COMMON_ORIGIN));
			m_pModelCom->Render(_uint(i));
		}
	}
	else if (m_eRenderType == RENDER_TYPE::MESH_CHECK)
	{
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (m_eType == CModel::TYPE::TYPE_ANIM)
				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
			m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
			m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

			if (i == m_iDiscardMeshNumber)
				m_pShaderCom->Begin(ECast(MONSTER_SHADER::COMMON_MESHCHECK));
			else
				m_pShaderCom->Begin(ECast(MONSTER_SHADER::COMMON_ORIGIN));

			m_pModelCom->Render(_uint(i));
		}

	}
	else if (m_eRenderType == RENDER_TYPE::RENDER_TEST)
	{
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (m_eType == CModel::TYPE::TYPE_ANIM)
				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
			m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
			m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));
			m_pShaderCom->Begin(m_iRenderPass);
			m_pModelCom->Render(_uint(i));
		}
	}

	return S_OK;
}

HRESULT CShaderParsed_Object::Render_Shadow()
{
	return S_OK;
}

HRESULT CShaderParsed_Object::Render_OutLine()
{
	return S_OK;
}

HRESULT CShaderParsed_Object::Bind_ShaderResources()
{
	return S_OK;
}

CShaderParsed_Object* CShaderParsed_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CShaderParsed_Object* pInstance = new CShaderParsed_Object(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CShaderParsed_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CShaderParsed_Object::Clone(void* pArg)
{
	CShaderParsed_Object* pInstance = new CShaderParsed_Object(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShaderParsed_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CShaderParsed_Object::Pool()
{
	return new CShaderParsed_Object(*this);
}

void CShaderParsed_Object::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
