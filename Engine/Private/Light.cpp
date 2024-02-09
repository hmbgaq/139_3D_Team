#include "..\Public\Light.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"

CLight::CLight()
{

}

HRESULT CLight::Initialize(const LIGHT_DESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	_uint		iPassIndex = { 0 };

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4));

		iPassIndex = 1;
	}
	else
	{
		pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4));
		pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float));

		iPassIndex = 2;
	}	

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(iPassIndex);

	pVIBuffer->Bind_VIBuffers();

	return pVIBuffer->Render();	
}

CLight * CLight::Create(const LIGHT_DESC & LightDesc)
{
	CLight*		pInstance = new CLight();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CLight::Free()
{
	__super::Free();

}
