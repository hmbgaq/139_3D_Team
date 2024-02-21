#include "stdafx.h"
#include "..\Public\Effect_Void.h"


CEffect_Void::CEffect_Void(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CAlphaObject(pDevice, pContext, strPrototypeTag)
{

}

CEffect_Void::CEffect_Void(const CEffect_Void & rhs)
	: CAlphaObject(rhs)
{
}

HRESULT CEffect_Void::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_Void::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	return S_OK;
}

void CEffect_Void::Priority_Tick(_float fTimeDelta)
{

	
}

void CEffect_Void::Tick(_float fTimeDelta)
{


}

void CEffect_Void::Late_Tick(_float fTimeDelta)
{

}

HRESULT CEffect_Void::Render()
{
	return S_OK;
}

void CEffect_Void::ReSet_Effect()
{


}

void CEffect_Void::End_Effect()
{


}



void CEffect_Void::Free()
{
	__super::Free();

}

