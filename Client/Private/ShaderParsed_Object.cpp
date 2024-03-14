#include "ShaderParsed_Object.h"
#include "GameInstance.h"
#include "Data_Manager.h"
#include "Model.h"

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
	FAILED_CHECK(Ready_Components());
}

HRESULT CShaderParsed_Object::Ready_Components()
{
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
		FAILED_CHECK(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
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

	return S_OK;
}

void CShaderParsed_Object::Priority_Tick(_float fTimeDelta)
{
}

void CShaderParsed_Object::Tick(_float fTimeDelta)
{
}

void CShaderParsed_Object::Late_Tick(_float fTimeDelta)
{
}

HRESULT CShaderParsed_Object::Render()
{
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
