#include "Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

_uint CLight::g_iLightIndex = 0;

CLight::CLight()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLight::Initialize(const LIGHT_DESC & LightDesc)
{
	m_LightDesc = LightDesc;

	m_LightDesc.iLightIndex = g_iLightIndex++;

	g_fCamFar = m_pGameInstance->Get_CamFar();

	return S_OK;
}

HRESULT CLight::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	if (!m_LightDesc.bEnable)
		return E_FAIL;

	_uint		iPassIndex = { 0 };

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4));

		iPassIndex = ECast(DEFERRED_SHADER::DIRECTIONAL_LIGHT);
	}
	else if (LIGHT_DESC::TYPE_POINT == m_LightDesc.eType)
	{
		pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4));
		pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float));

		iPassIndex = ECast(DEFERRED_SHADER::POINT_LIGHT);
	}	
	else if (LIGHT_DESC::TYPE_SPOTLIGHT == m_LightDesc.eType)
	{
		iPassIndex = ECast(DEFERRED_SHADER::SPOT_LIGHT);
	}

	FAILED_CHECK(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4)));
	FAILED_CHECK(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4)));
	FAILED_CHECK(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4)));

	pShader->Begin(iPassIndex);

	pVIBuffer->Bind_VIBuffers();

	return pVIBuffer->Render();	
}

void CLight::Set_Diffuse(_float4 vDiffuse)
{
	m_LightDesc.vDiffuse = vDiffuse;
}

void CLight::Set_Specular(_float4 vSpecular)
{
	m_LightDesc.vSpecular = vSpecular;
}

void CLight::Set_Ambient(_float4 vAmbient)
{
	m_LightDesc.vAmbient = vAmbient;
}

void CLight::Decrease_GlobalIndex()
{
	--g_iLightIndex;
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

	Safe_Release(m_pGameInstance);
}
