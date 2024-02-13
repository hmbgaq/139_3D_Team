#include "stdafx.h"
#include "..\Public\Effect_Object.h"

#include "GameInstance.h"


CEffect_Object::CEffect_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAlphaObject(pDevice, pContext)
{

}

CEffect_Object::CEffect_Object(const CEffect_Object & rhs)
	: CAlphaObject(rhs)
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


void CEffect_Object::Free()
{
	__super::Free();

}

