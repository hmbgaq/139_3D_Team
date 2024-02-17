#include "stdafx.h"
#include "..\Public\Effect_Object.h"

#include "GameInstance.h"


CEffect_Object::CEffect_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CEffect_Object::CEffect_Object(const CEffect_Object & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Object::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_Object::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	

	return S_OK;
}

void CEffect_Object::Priority_Tick(_float fTimeDelta)
{

	
}

void CEffect_Object::Tick(_float fTimeDelta)
{


}

void CEffect_Object::Late_Tick(_float fTimeDelta)
{

}

HRESULT CEffect_Object::Render()
{

	return S_OK;
}

CEffect_Object* CEffect_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Object* pInstance = new CEffect_Object(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CEffect_Object::Clone(void* pArg)
{
	CEffect_Object* pInstance = new CEffect_Object(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Object::Free()
{
	__super::Free();

}

